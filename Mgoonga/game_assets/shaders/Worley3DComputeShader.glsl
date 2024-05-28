#version 450

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout (rgba32f, binding = 0) uniform image3D destTex;

layout (std430, binding = 1) buffer Points1 {
    vec4 points1[];
};
layout (std430, binding = 2) buffer Points2 {
    vec4 points2[];
};
layout (std430, binding = 3) buffer Points3 {
    vec4 points3[];
};

uniform int octaveOneSize = 8;
uniform int octaveTwoSize = 12;
uniform int octaveThreeSize = 16;

uniform int gamma = 142; // Adjust this value for desired contrast enhancement

float distanceSquared(vec3 p1, vec3 p2)
{
    vec3 d = p1 - p2;
    return dot(d, d);
}

float GetNoiseValue(int octaveSize);

void main()
{
	ivec3 texelPos = ivec3(gl_GlobalInvocationID.xyz);
	float val1 = GetNoiseValue(octaveOneSize);
	float val2 = GetNoiseValue(octaveTwoSize);
	float val3 = GetNoiseValue(octaveThreeSize);
	float alpha = imageLoad(destTex, texelPos).a;
    imageStore(destTex, texelPos, vec4(val1, val2, val3, alpha));	
}

float GetNoiseValue(int octaveSize)
{
	ivec3 texelPos = ivec3(gl_GlobalInvocationID.xyz);
    ivec3 imageSize = ivec3(imageSize(destTex));
    vec3 texCoord = vec3(texelPos) / vec3(imageSize);

	ivec3 subcubeSize = imageSize / octaveSize;
    ivec3 subCubeIndex = texelPos / subcubeSize;

    float minDist = 1.0;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
			
                ivec3 neighborIndex = subCubeIndex + ivec3(dx, dy, dz);

                // Wrap neighbor indices to create periodic boundaries
                ivec3 wrappedIndex = (neighborIndex + octaveSize) % octaveSize;

                int index = wrappedIndex.x +
                            wrappedIndex.y * octaveSize +
                            wrappedIndex.z * octaveSize * octaveSize;

				vec3 point;
				if(octaveSize == octaveOneSize)
					point = vec3(points1[index]);
				else if(octaveSize == octaveTwoSize)
					point =  vec3(points2[index]);
				else if(octaveSize == octaveThreeSize)
					point = vec3(points3[index]);
				
				point += (neighborIndex - wrappedIndex) / octaveSize;				
				float dist = distanceSquared(texCoord, point);	
                minDist = min(minDist, dist);
            }
        }
    }	
	return pow(1.0 - minDist, gamma);
}
#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout (rgba32f, binding = 0) uniform image3D destTex;
layout (std430, binding = 1) buffer Points {
    vec4 points[];
};

uniform int octaveOneSize = 8;
uniform float gamma = 42.0; // Adjust this value for desired contrast enhancement

float distanceSquared(vec3 p1, vec3 p2)
{
    vec3 d = p1 - p2;
    return dot(d, d);
}

void main()
{
    ivec3 texelPos = ivec3(gl_GlobalInvocationID.xyz);
    ivec3 imageSize = ivec3(imageSize(destTex));
    vec3 texCoord = vec3(texelPos) / vec3(imageSize);

	ivec3 subcubeSize = imageSize / octaveOneSize;
    ivec3 subCubeIndex = texelPos / subcubeSize;

    float minDist = 1.0;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                ivec3 neighborIndex = subCubeIndex + ivec3(dx, dy, dz);

                if (neighborIndex.x >= 0 && neighborIndex.x < octaveOneSize &&
                    neighborIndex.y >= 0 && neighborIndex.y < octaveOneSize &&
                    neighborIndex.z >= 0 && neighborIndex.z < octaveOneSize)
				{
                    int index = neighborIndex.x +
                                neighborIndex.y * octaveOneSize +
                                neighborIndex.z * octaveOneSize * octaveOneSize;

                    float dist = distanceSquared(texCoord, vec3(points[index]));
                    minDist = min(minDist, dist);
                }
            }
        }
    }
	
	float val = pow(1.0 - minDist, gamma);
    imageStore(destTex, texelPos, vec4(val));	
}
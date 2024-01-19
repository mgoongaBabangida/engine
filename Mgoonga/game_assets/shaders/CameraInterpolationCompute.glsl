#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform mat4 invView;
uniform mat4 invProj;

uniform mat4 view;
uniform mat4 proj;

uniform float width = 1200.f;
uniform float height = 600.f;

layout(rgba32f, binding = 0) writeonly uniform image2D imgOutput;
layout(binding = 1) uniform sampler2D screenTexture;
layout(binding = 2) uniform sampler2D tPos;

void main() 
{
	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy); // Use global invocation ID as texture coordinate
	vec2 texCoords = vec2(float(texelCoord.x)/width, float(texelCoord.y)/height );
	
	vec3 pos = texture(tPos, texCoords).xyz;
	vec4 projectedPos = inverse(invProj) * vec4(pos, 1.0f);
	projectedPos /= projectedPos.w;
	if(pos.z == 0.0f)
		projectedPos.z = 1.0f;
	vec3 rawPosition = vec3(texCoords, projectedPos.z);
	// Convert from (0, 1) range to (-1, 1)
    vec4 ScreenSpacePosition = vec4( rawPosition * 2.0f - 1.0f, 1.0f);
	
	vec4 ViewPosition = invProj * ScreenSpacePosition;
	ViewPosition /= ViewPosition.w;	
	vec4 worldPos = invView * ViewPosition;
	
	vec4 ProjectedPosition = proj * view * worldPos;
	ProjectedPosition /= ProjectedPosition.w;
	
	// Convert from range (-1, 1) to (0, 1)
	vec4 newCoords = vec4(ProjectedPosition.xyz * 0.5f + 0.5f, 1.0f);
	
	if(newCoords.x >= 0 && newCoords.y >=0 && newCoords.x <= 1 && newCoords.y <= 1)
	{
	 ivec2 newCoordsI = ivec2(newCoords.x * width, newCoords.y * height);
	 vec4 pixelValue = texture(screenTexture, texCoords);
	 imageStore(imgOutput, newCoordsI, pixelValue);
	}
}
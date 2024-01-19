#version 430

out vec4 newCoords;

uniform mat4 invView;
uniform mat4 invProj;

uniform mat4 view;
uniform mat4 proj;

layout(binding=2) uniform sampler2D tPos;
layout(binding=3) uniform sampler2D tDepth;

in vec2 TexCoords;

vec3 CalcViewPosition(in vec2 TexCoord)
{
    // Combine UV & depth into XY & Z (NDC)
    vec3 rawPosition                = vec3(TexCoord, texture(tDepth, TexCoord).r);

    // Convert from (0, 1) range to (-1, 1)
    vec4 ScreenSpacePosition        = vec4( rawPosition * 2 - 1, 1);

    // Undo Perspective transformation to bring into view space
    vec4 ViewPosition               = invProj * ScreenSpacePosition;

    // Perform perspective divide and return
    return                          ViewPosition.xyz / ViewPosition.w;
}

vec3 PositionFromDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProj * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

void main()
{
	vec3 pos = texture(tPos, TexCoords).xyz;
	vec4 projectedPos = inverse(invProj) * vec4(pos, 1.0f);
	projectedPos /= projectedPos.w;
	if(pos.z == 0.0f)
		projectedPos.z = 1.0f;
	vec3 rawPosition = vec3(TexCoords, projectedPos.z);
	// Convert from (0, 1) range to (-1, 1)
    vec4 ScreenSpacePosition = vec4( rawPosition * 2.0f - 1.0f, 1.0f);
	
	vec4 ViewPosition = invProj * ScreenSpacePosition;
	ViewPosition /= ViewPosition.w;	
	vec4 worldPos = invView * ViewPosition;
	
	vec4 ProjectedPosition = proj * view * worldPos;
	ProjectedPosition /= ProjectedPosition.w;
	
	// Convert from range (-1, 1) to (0, 1)
	newCoords = vec4(ProjectedPosition.xyz * 0.5f + 0.5f, 1.0f);
}
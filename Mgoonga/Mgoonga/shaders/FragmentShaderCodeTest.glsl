#version 430

out vec4 daColor;

layout(binding=1) uniform sampler2DShadow depth_texture; // Shadow
layout(binding=2) uniform sampler2D texture_diffuse1;
layout(binding=3) uniform sampler2D texture_specular1;
layout(binding=4) uniform sampler2D texture_normal1;
layout(binding=5) uniform sampler2D texture_depth1;

void main()
{   
	daColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
};
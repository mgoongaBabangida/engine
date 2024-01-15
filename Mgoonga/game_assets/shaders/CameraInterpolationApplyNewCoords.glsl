#version 430

out vec4 newImage;

layout(binding=2) uniform sampler2D tImage;
layout(binding=3) uniform sampler2D tNewCoords;

in vec2 TexCoords;

void main()
{
	vec3 newCoords = texture(tNewCoords, TexCoords).xyz; // NDC
	
    // Sample from the image using the corrected newCoords
    newImage = texture(tImage, newCoords.xy);
}
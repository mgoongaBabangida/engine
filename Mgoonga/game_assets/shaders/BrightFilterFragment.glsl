#version 430

layout(binding=1) uniform sampler2D originalTexture;

out vec4 out_colour;
in vec2 TexCoords;
//in vec2 blurTextureCoords[11];

void main(void)
{
	 vec4 color = texture(originalTexture, TexCoords);
	 float brightness = (color.r * 0.2126) + (color.g * 0.7152) + (color.b * 0.0722);
	 out_colour = color * brightness;

};
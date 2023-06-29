#version 430

layout(binding=1) uniform sampler2D originalTexture;

out vec4 out_colour;
//in vec2 TexCoords;
in vec2 blurTextureCoords[11];

void main(void)
{	
	out_colour = vec4(0.0);
	out_colour += texture(originalTexture, blurTextureCoords[0]) * 0.0093f;
    out_colour += texture(originalTexture, blurTextureCoords[1]) * 0.028002f;
    out_colour += texture(originalTexture, blurTextureCoords[2]) * 0.065984f;
    out_colour += texture(originalTexture, blurTextureCoords[3]) * 0.121703f;
    out_colour += texture(originalTexture, blurTextureCoords[4]) * 0.175713f;
    out_colour += texture(originalTexture, blurTextureCoords[5]) * 0.198596f;
    out_colour += texture(originalTexture, blurTextureCoords[6]) * 0.175713f;
    out_colour += texture(originalTexture, blurTextureCoords[7]) * 0.121703f;
    out_colour += texture(originalTexture, blurTextureCoords[8]) * 0.065984f;
    out_colour += texture(originalTexture, blurTextureCoords[9]) * 0.028002f;
    out_colour += texture(originalTexture, blurTextureCoords[10]) * 0.0093f;
}
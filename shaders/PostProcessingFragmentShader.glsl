#version 430

in vec2 TexCoords;
out vec4 color;
uniform bool frame = false;
uniform bool blend = false;
layout(binding=1) uniform sampler2D screenTexture;
layout(binding=2) uniform sampler2D contrastTexture;

void main()
{
	vec4 col;
	vec4 contr;
	if( frame )
	 col = vec4(1.0, 1.0, 0.0, 1.0);
	else if(blend)
	 {
	   col = texture(screenTexture, TexCoords);
	   contr = texture(contrastTexture, TexCoords);
	   col = col + contr;
	 }
    else
    col = texture(screenTexture, TexCoords);
   color = col;
}
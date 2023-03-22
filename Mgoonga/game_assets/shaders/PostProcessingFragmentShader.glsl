#version 430

in vec2 TexCoords;
out vec4 color;
uniform bool frame = false;
uniform bool blend = false;
uniform bool kernel = false;
uniform float blurCoef = 1.0f;
layout(binding=1) uniform sampler2D screenTexture;
layout(binding=2) uniform sampler2D contrastTexture;

void main()
{
	vec4 col;
	vec4 contr;
	if( frame )
	 col = vec4(1.0, 1.0, 0.0, 1.0);
	else if( blend )
	{
	   col = texture(screenTexture, TexCoords);
	   contr = texture(contrastTexture, TexCoords);
	   col = col + (contr * clamp(blurCoef,0,1));
	}
	else if( kernel )
	{
		col = texture(screenTexture, TexCoords);
		float average = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
		col = vec4(average, average, average, 1.0);
	}
    else
	   col = texture(screenTexture, TexCoords);
   color = col;
}
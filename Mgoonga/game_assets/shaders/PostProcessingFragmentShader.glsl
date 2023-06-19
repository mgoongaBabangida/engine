#version 430

in vec2 TexCoords;

out vec4 color;

uniform bool frame = false;
uniform bool blend = false;
uniform bool kernel = false;

uniform float blurCoef = 1.0f;
uniform vec2 CursorPos;

layout(binding=1) uniform sampler2D screenTexture;
layout(binding=2) uniform sampler2D contrastTexture;
layout(binding=3) uniform sampler2D alphaMask;

subroutine vec4 ColorCalculationPtr();
subroutine uniform ColorCalculationPtr ColorFunction;

subroutine(ColorCalculationPtr) vec4 TestColor()
{
	float dist = distance(gl_FragCoord.xy, CursorPos);
	float alpha = texture(alphaMask, TexCoords).a;
	if(dist < 30.0f && alpha > 0.0f)
		return texture(screenTexture, TexCoords);
	else
		return texture(screenTexture, TexCoords) * 0.9f;
}	

subroutine(ColorCalculationPtr) vec4 GreyKernelColor()
{
	vec4 col = texture(screenTexture, TexCoords);
	float average = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
	return vec4(average, average, average, 1.0);
}

subroutine(ColorCalculationPtr) vec4 DefaultColor()
{
 return texture(screenTexture, TexCoords);
}

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
	{
		col = ColorFunction();
	}
   color = col;
}
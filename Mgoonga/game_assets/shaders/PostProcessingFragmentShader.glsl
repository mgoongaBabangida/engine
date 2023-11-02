#version 430

in vec2 TexCoords;

out vec4 color;

uniform bool frame = false;
uniform bool blend = false;
uniform bool kernel = false;

uniform float blurCoef = 1.0f;
uniform bool tone_mapping = true;
uniform bool gamma_correction = true;
uniform float hdr_exposure = 1.0f;

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

subroutine(ColorCalculationPtr) vec4 MaskBlendColor()
{
	vec4 maincol = texture(screenTexture, TexCoords);
	vec4 contrastcol = texture(contrastTexture, TexCoords);
	float maskcol = texture(alphaMask, TexCoords).r;
	return contrastcol * maskcol + maincol * (1 - maskcol);
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
	   if(tone_mapping)
		col.rgb = vec3(1.0) - exp(-col.rgb * hdr_exposure);	 
	   if(gamma_correction)
		col.rgb = pow(col.rgb, vec3(1.0/2.2f));
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
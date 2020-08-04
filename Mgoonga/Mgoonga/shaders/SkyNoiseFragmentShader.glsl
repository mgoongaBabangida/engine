#version 430

#define PI 3.14159265359

uniform vec4 SkyColor = vec4(0.1, 0.1, 0.9, 1.0);
uniform vec4 CloudColor = vec4(1.0, 1.0, 1.0, 1.0);

layout(binding=4) uniform sampler2D texture_normal1; //change name normalTexture

in vec2 TextureCoords;

out vec4 out_Color;

void main()
{
	vec4 noise = texture(texture_normal1,TextureCoords);
	float t = (cos(noise.a*PI)+1.0)/2.0;
	vec4 color = mix(SkyColor,CloudColor,t);
	color.a =color.r*color.r/2 + color.g*color.g/2 + color.b*color.b/2;
	color.r = clamp(color.r *1.5f, 0.0f, 1.0f);
	color.g = clamp(color.g *1.5f, 0.0f, 1.0f);
	out_Color =vec4(color);
}
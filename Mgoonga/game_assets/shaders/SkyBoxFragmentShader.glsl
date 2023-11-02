#version 430 core

layout (location = 0) out vec4 color;
layout (location = 1) out float mask;
in vec3 TexCoords;

uniform bool gamma_correction = true;

layout(binding=5) uniform samplerCube skybox;

void main()
{    
   color = texture(skybox, TexCoords);
   if(gamma_correction == true)
	color.rgb = pow(color.rgb, vec3(2.2));
   mask = 0.0f;
}
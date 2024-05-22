#version 430

in vec2 TexCoords;

out vec4 color;

layout(binding=0) uniform sampler3D noiseTexture;

uniform float z_slice = 0.5f;
uniform int debug_octave = 1;

void main()
{
    color = texture(noiseTexture, vec3(TexCoords, z_slice)); // Slice the 3D texture at z = 0.5
	color = vec4(color[debug_octave],color[debug_octave],color[debug_octave],1.f);
}
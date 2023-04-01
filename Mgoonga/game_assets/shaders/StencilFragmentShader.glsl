#version  440 core

out vec4 outColor;

uniform vec3 color = vec3(1.0, 1.0, 0.0);

void main()
{
    outColor = vec4(color, 1.0);
}

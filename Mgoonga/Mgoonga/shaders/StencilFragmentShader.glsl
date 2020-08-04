#version  430 core

out vec4 daColor;

uniform sampler2D screenTexture;

void main()
{
    daColor = vec4(1.0, 1.0, 0.0, 1.0);
}

#version 430 core

in float Height;

out vec4 FragColor;

void main()
{
    FragColor = vec4(150.f/255.f * Height, 75.f/255.f * Height, 0.f, 1.f);
}
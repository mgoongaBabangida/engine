#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

uniform mat4 model;

out vec2 TexCoord;
out vec3 thePositionWorld;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTex;
    thePositionWorld = vec3(model * vec4(aPos, 1.0f));

}
#version  430

in layout(location=0) vec2 position;
in layout(location=1) vec2 texcoord;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
    TexCoords = texcoord;
};
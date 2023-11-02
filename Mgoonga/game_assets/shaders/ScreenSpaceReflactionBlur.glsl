#version 430

out vec4 FragColor;
  
in vec2 TexCoords;
  
layout(binding=2) uniform sampler2D ssaoInput;

void main() 
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    vec4 result = vec4(0.0);
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).rgba;
        }
    }
    FragColor = result / (4.0 * 4.0);
}  
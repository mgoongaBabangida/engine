#version  440 core

in vec2 toDiscard;

out vec4 outColor;

uniform vec4 color = vec4(1.0, 1.0, 0.0, 1.0);

void main()
{
 if(toDiscard == vec2(1.0f, 1.0f))
	discard;
 
 outColor = color;
}

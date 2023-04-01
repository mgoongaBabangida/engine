#version 440 core

layout (vertices = 4) out;

uniform int NumSegments;
uniform int NumStrips;

void main()
{
gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

gl_TessLevelOuter[0] = float(NumStrips);
gl_TessLevelOuter[1] = float(NumSegments);
}
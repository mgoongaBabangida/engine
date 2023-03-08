#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 7) out;

uniform mat4 MVP; 
uniform float radius;

void build_hex(vec4 position)
{    
    gl_Position = position +  MVP * vec4(0.866, 0.0, 0.5, 0.0) * radius ;    // 1:bottom-left
    EmitVertex();   
    gl_Position = position +  MVP * vec4( 0.0, 0.0, 1.0,  0.0) * radius;    // 2:bottom-right
    EmitVertex();
    gl_Position = position +  MVP * vec4(-0.866, 0.0,  0.5,  0.0) * radius;    // 3:top-left
    EmitVertex();
    gl_Position = position +  MVP * vec4( -0.866, 0.0, -0.5,  0.0) * radius;    // 4:top-right
    EmitVertex();
    gl_Position = position +  MVP * vec4( 0.0, 0.0, -1.0,  0.0) * radius;    // 5:top
    EmitVertex();
	gl_Position = position +  MVP * vec4( 0.866, 0.0, -0.5,  0.0) * radius;    // 6:top
    EmitVertex();
	gl_Position = position +  MVP * vec4(0.866, 0.0, 0.5, 0.0) * radius;     // 7:top
    EmitVertex();
    EndPrimitive();
}

void main() {    
    build_hex(gl_in[0].gl_Position);
} 
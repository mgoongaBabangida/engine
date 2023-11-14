#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 37) out;

uniform mat4 MVP; 
uniform float radius;
uniform int type = 0;

out vec2 toDiscard;

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

void build_circle(vec4 position)
{
	gl_Position = position +  MVP * vec4(0.98480775, 0.0, 0.17364818, 0.0) * radius ;
		EmitVertex();   
	gl_Position = position +  MVP * vec4(0.93969262, 0.0, 0.34202014, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(0.8660254, 0.0, 0.5, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(0.76604444, 0.0, 0.64278761, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(0.64278761, 0.0, 0.76604444, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(0.5, 0.0, 0.8660254, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(0.34202014, 0.0, 0.93969262, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(0.17364818, 0.0, 0.98480775, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(0.0, 0.0, 1.0, 0.0) * radius ;
		EmitVertex(); 
		
	gl_Position = position +  MVP * vec4(-0.17364818, 0.0, 0.98480775, 0.0) * radius ;
       EmitVertex(); 
    gl_Position = position +  MVP * vec4(-0.34202014, 0.0,0.93969262, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(-0.5, 0.0, 0.8660254, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(-0.64278761, 0.0, 0.76604444, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(-0.76604444, 0.0, 0.64278761, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(-0.8660254, 0.0, 0.5, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(-0.93969262, 0.0, 0.34202014, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(-0.98480775, 0.0, 0.17364818, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(-1, 0.0, 0.0, 0.0) * radius ;
    EmitVertex();

	gl_Position = position +  MVP * vec4(-0.98480775, 0.0, -0.17364818, 0.0) * radius ;
		EmitVertex();   
	gl_Position = position +  MVP * vec4(-0.93969262, 0.0, -0.34202014, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(-0.8660254, 0.0, -0.5, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(-0.76604444, 0.0, -0.64278761, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(-0.64278761, 0.0, -0.76604444, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(-0.5, 0.0, -0.8660254, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(-0.34202014, 0.0, -0.93969262, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(-0.17364818, 0.0, -0.98480775, 0.0) * radius ;
		EmitVertex();  
	gl_Position = position +  MVP * vec4(0.0, 0.0, -1.0, 0.0) * radius ;
		EmitVertex(); 
		
	gl_Position = position +  MVP * vec4(0.17364818, 0.0, -0.98480775, 0.0) * radius ;
       EmitVertex(); 
    gl_Position = position +  MVP * vec4(0.34202014, 0.0, -0.93969262, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(0.5, 0.0, -0.8660254, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(0.64278761, 0.0, -0.76604444, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(0.76604444, 0.0, -0.64278761, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(0.8660254, 0.0, -0.5, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(0.93969262, 0.0, -0.34202014, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(0.98480775, 0.0, -0.17364818, 0.0) * radius ;
        EmitVertex(); 
    gl_Position = position +  MVP * vec4(1, 0.0, 0.0, 0.0) * radius ;
		EmitVertex();
	
	gl_Position = position +  MVP * vec4(0.98480775, 0.0, 0.17364818, 0.0) * radius ;
		EmitVertex();
}

void main() 
{    
	toDiscard = vec2(0.f,0.f);
	if(type == 0)
		build_hex(gl_in[0].gl_Position);
	else if(type == 1)
		build_circle(gl_in[0].gl_Position);
} 

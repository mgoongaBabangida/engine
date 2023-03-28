// Vertex shader for shadow map generation
 #version 430 
 
 uniform mat4 MVP; 
 in layout (location = 0) vec3 position;
 
 void main(void)
 { 
    vec4 v = vec4(position ,1.0);
    gl_Position = MVP * v;
 };
// Vertex shader for shadow map generation
 #version 430 core
 in layout (location = 0) vec3 position;

 uniform mat4 MVP;
 
 out vec2 toDiscard;
 
 void main(void)
 { 
    vec4 v = vec4(position ,1.0);
    toDiscard = vec2(0.f,0.f);
    gl_Position = MVP * v;
 };
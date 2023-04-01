 #version 440 core
  
 in layout (location = 0) vec3 position;
 
 void main(void)
 { 
    gl_Position = vec4(position ,1.0);
 };
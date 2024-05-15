#version 430
layout(location = 0) in vec3 aPos;

 uniform mat4 MVP;
 
out vec3 normalizedNoisePos;

void main()
{
    // Transform from [-1,1] to [0,1] should always be in [0,1] space !
    normalizedNoisePos = (aPos + 1.0) / 2.0;
	
	vec4 v = vec4(aPos ,1.0);
    gl_Position = MVP * v;
}
#version 430
layout(location = 0) in vec3 aPos;

 uniform mat4 MVP;
 uniform mat4 modelMatrix;
 uniform mat4 localToNoiseSpace;
 
out vec3 normalizedNoisePos;
out vec4 worldPos;

void main()
{
    // Transform from [-1,1] to [0,1] should always be in [0,1] space !
    normalizedNoisePos = vec3(localToNoiseSpace * vec4(aPos, 1.0f)); //(aPos + 1.0) / 2.0;
	
	vec4 v = vec4(aPos , 1.0f);
	worldPos = modelMatrix * v;
    gl_Position = MVP * v;
}
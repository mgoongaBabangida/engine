#version 460 core
layout (location = 0) in vec3 position;

in layout(location=6) ivec4 boneIDs;
in layout(location=7) vec4 weights;
 
const int MAX_BONES = 100;
 
uniform mat4 model;
uniform mat4 gBones[MAX_BONES];
 
void main()
{
  mat4 BoneTransform      = gBones[boneIDs[0]] * weights[0];
       BoneTransform     += gBones[boneIDs[1]] * weights[1];
       BoneTransform     += gBones[boneIDs[2]] * weights[2];
       BoneTransform     += gBones[boneIDs[3]] * weights[3];
	 
	vec4 v =  BoneTransform * vec4(position ,1.0);	 
    gl_Position = model * v;
}
#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normal;
in layout(location=3) vec2 texcoord;
in layout(location=4) vec3 tangent;
in layout(location=5) vec3 bitangent;
in layout(location=6) ivec4 boneIDs;
in layout(location=7) vec4 weights;

const int MAX_BONES = 100;

uniform mat4 modelToProjectionMatrix;
uniform mat4 gBones[MAX_BONES];

void main()
{
 mat4 matrix = mat4(vec4(1,0,0,0),vec4(0,1,0,0),vec4(0,0,1,0),vec4(0,0,0,1));
	
    mat4 BoneTransform = gBones[boneIDs[0]] * weights[0];
    BoneTransform     += gBones[boneIDs[1]] * weights[1];
    BoneTransform     += gBones[boneIDs[2]] * weights[2];
    BoneTransform     += gBones[boneIDs[3]] * weights[3];
	
  vec4 v =  BoneTransform * vec4(position ,1.0);
  gl_Position	= modelToProjectionMatrix * v;
};

#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

in layout(location=6) ivec4 boneIDs;
in layout(location=7) vec4 weights;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

const int MAX_BONES = 100;

uniform bool invertedNormals = false;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];

void main()
{
  mat4 BoneTransform      = gBones[boneIDs[0]] * weights[0];
       BoneTransform     += gBones[boneIDs[1]] * weights[1];
       BoneTransform     += gBones[boneIDs[2]] * weights[2];
       BoneTransform     += gBones[boneIDs[3]] * weights[3];

	vec4 pos =  BoneTransform * vec4(aPos, 1.0);

    vec4 viewPos = view * model * pos;
    FragPos = viewPos.xyz; 
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    Normal = normalMatrix * (invertedNormals ? -aNormal : aNormal);
    
    gl_Position = projection * viewPos;
}
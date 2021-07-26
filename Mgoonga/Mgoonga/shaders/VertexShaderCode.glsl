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
uniform mat4 modelToWorldMatrix;
uniform mat4 shadowMatrix;
uniform vec4 clip_plane;
uniform mat4 gBones[MAX_BONES];

out vec3 thePosition;
out vec3 theNormal;
out vec2 Texcoord;
out vec4 LightSpacePos;
out mat3 TBN;
out vec4 debug;

void main()
{
    mat4 matrix = mat4(vec4(1,0,0,0),vec4(0,1,0,0),vec4(0,0,1,0),vec4(0,0,0,1));
	
    mat4 BoneTransform      = gBones[boneIDs[0]] * weights[0];
         BoneTransform     += gBones[boneIDs[1]] * weights[1];
         BoneTransform     += gBones[boneIDs[2]] * weights[2];
         BoneTransform     += gBones[boneIDs[3]] * weights[3];
	
  vec4 v =  BoneTransform * vec4(position ,1.0); // BoneTransform *
 
  gl_Position	= modelToProjectionMatrix * v;
  LightSpacePos = shadowMatrix * modelToWorldMatrix * v;

  vec4 WorldPosV	    = modelToWorldMatrix * v;	  //water
  gl_ClipDistance[0]	= dot(WorldPosV,clip_plane); //water

  Texcoord = texcoord;

  vec3 ntangent		= mat3(BoneTransform) * tangent;
  vec3 nbitangent	= mat3(BoneTransform) * bitangent;
  vec3 nnormal		= mat3(BoneTransform) * normal;

  theNormal		= normalize(mat3(modelToWorldMatrix)* normal);
  thePosition	= vec3(modelToWorldMatrix * v);// vec4(position ,1.0)

  vec3 T = normalize(vec3(modelToWorldMatrix * vec4(tangent,   0.0)));
  vec3 B = normalize(vec3(modelToWorldMatrix * vec4(bitangent, 0.0)));
  vec3 N = normalize(vec3(modelToWorldMatrix * vec4(normal,    0.0)));
  if (dot(cross(N, T), B) < 0.0)
                T = T * -1.0;
  TBN = mat3(T,B,N);
  
  float t = weights[0] + weights[1] + weights[2] + weights[3];
  debug = vec4(t, t, t, 1.0f);
};

#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normal;
in layout(location=3) vec2 texcoord;
in layout(location=4) vec3 tangent;
in layout(location=5) vec3 bitangent;

uniform mat4 modelToProjectionMatrix[10];
uniform mat4 modelToWorldMatrix[10];

uniform mat4 shadowMatrix;

out vec3 thePosition;
out vec3 theNormal;
out vec2 Texcoord;
out vec4 LightSpacePos;
out mat3 TBN;

void main()
{	
  vec4 v = vec4(position ,1.0);
 
  gl_Position	= modelToProjectionMatrix[gl_InstanceID] * v;
  LightSpacePos = shadowMatrix * modelToWorldMatrix[gl_InstanceID] * v;

  Texcoord = texcoord;
  theNormal	= normalize(mat3(modelToWorldMatrix[gl_InstanceID])* normalize(normal));
  thePosition	= vec3(modelToWorldMatrix[gl_InstanceID] * v);// vec4(position ,1.0) 

  vec3 T = normalize(vec3(modelToWorldMatrix[gl_InstanceID] * vec4(tangent,   0.0)));
  vec3 B = normalize(vec3(modelToWorldMatrix[gl_InstanceID] * vec4(bitangent, 0.0)));
  vec3 N = normalize(vec3(modelToWorldMatrix[gl_InstanceID] * vec4(normal,    0.0)));
  
  if (dot(cross(N, T), B) < 0.0)
                T = T * -1.0;
  TBN = mat3(T,B,N);  
};

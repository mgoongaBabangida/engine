#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normal;
in layout(location=3) vec2 texcoord;


uniform mat4 modelToProjectionMatrix;
uniform mat4 modelToWorldMatrix;
uniform mat4 shadowMatrix;

out vec3 thePosition;
out vec3 theNormal;
out vec2 Texcoord;
out vec4 LightSpacePos;


void main()
{	
  vec4 v = vec4(position ,1.0);
 
  gl_Position	= modelToProjectionMatrix * v;
  LightSpacePos = shadowMatrix * modelToWorldMatrix * v;

  Texcoord = texcoord;
  theNormal		= normalize(mat3(modelToWorldMatrix)* normalize(normal));
  thePosition	= vec3(modelToWorldMatrix * v);// vec4(position ,1.0)   
};

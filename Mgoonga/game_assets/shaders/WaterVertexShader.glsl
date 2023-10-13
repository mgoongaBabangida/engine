#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normal;
in layout(location=3) vec2 texcoord;
in layout(location=4) vec3 tangent;
in layout(location=5) vec3 bitangent;

uniform mat4 modelToProjectionMatrix;
uniform mat4 modelToWorldMatrix;
uniform vec4 cameraPosition;
uniform vec4 lightPosition;

out vec4 ClipSpaceCoord;
out vec2 TextureCoords;
out vec3 ToCameraVector;
out vec3 FromLightVector;

uniform float tiling = 6.0f;

void main()
{
  vec4 v = vec4(position ,1.0);
  vec4 ClipSpaceCoord = modelToProjectionMatrix * v;
  gl_Position	      = ClipSpaceCoord;
  
  //TextureCoords	      = texcoord;
  TextureCoords = vec2(texcoord.x/2 + 0.5,texcoord.y/2 + 0.5) * tiling;
  
  vec4 worldPositon   = modelToWorldMatrix * vec4(position ,1.0);
  ToCameraVector      = cameraPosition.xyz - worldPositon.xyz;
  FromLightVector     = worldPositon.xyz - lightPosition.xyz;
};

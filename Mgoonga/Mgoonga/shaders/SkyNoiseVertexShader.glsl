#version 430
in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normal;
in layout(location=3) vec2 texcoord;
in layout(location=4) vec3 tangent;
in layout(location=5) vec3 bitangent;

uniform mat4 modelToProjectionMatrix;
uniform float moveFactor;

out vec2 TextureCoords;

void main()
{
  vec4 v = vec4(position ,1.0);
  TextureCoords = texcoord;
  TextureCoords.y += moveFactor;
  gl_Position= modelToProjectionMatrix * v;
};

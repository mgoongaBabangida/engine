#version 430
 
in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normal;
in layout(location=3) vec2 texcoord;
in layout(location=4) vec3 tangent;
in layout(location=5) vec3 bitangent;
 
out vec3 thePosition;
out vec3 theNormal; 
out vec2 Texcoord;
out vec4 LightSpacePos;
out vec4 LocalSpacePos;
out vec3 LocalSpaceNormal;
out mat3 TBN;
 
uniform float Time;
uniform float Freq = 2.5;
uniform float Velocity = 2.5;
uniform float Amp = 0.6;

uniform mat4 modelToWorldMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat4 shadowMatrix;

void main()
{
     vec4 pos = vec4(position,1.0);
     float u = Freq * pos.x - Velocity * Time;
     pos.y = Amp * sin( u );
 
	 vec3 n = vec3(0.0);
	 n.xy = normalize(vec2(cos( u ), 1.0));
 
	 thePosition = mat3(ModelViewMatrix) * position; 
	 theNormal = normalize(mat3(modelToWorldMatrix)* n);
	 Texcoord = texcoord;
	 LightSpacePos = shadowMatrix * modelToWorldMatrix* pos;
	 LocalSpacePos = vec4(position, 1.0f);
	 LocalSpaceNormal = normal;
     gl_Position = MVP * pos;
} 

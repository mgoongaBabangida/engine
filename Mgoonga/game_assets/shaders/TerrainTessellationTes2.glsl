#version 430 core
layout(quads, equal_spacing, cw) in;

layout(binding=2) uniform sampler2D heightMap;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float height_scale = 1.75f;
uniform float max_height = 1.75f;
uniform float min_height = 0.f;

uniform float heightMapResoultion = 2056.f;
uniform float terrainSizeXZ = 12.7f;

in vec2 TextureCoord[];

out float Height;
out vec2 texCoord;
out vec4 LocalSpacePos;
out mat3 TBN;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    texCoord = (t1 - t0) * v + t0;

    Height = texture(heightMap, texCoord).x;
    Height *= height_scale;
    if(Height > max_height)
		Height = max_height;
		
	if(Height < min_height)
		Height = min_height;

    // ----------------------------------------------------------------------
    // retrieve control point position coordinates
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = vec4( 0.f, 1.f, 0.f, 0.f);

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0 + normal * Height;
	LocalSpacePos = p;
	
	gl_Position = projection * view * model * p;
		
	float uTexelSize = 1.0f / heightMapResoultion;
	float heightRight = texture(heightMap, texCoord + vec2( uTexelSize, 0.0)).x;
	heightRight *= height_scale;
    if(heightRight > max_height)
		heightRight = max_height;
	float heightUp    = texture(heightMap, texCoord + vec2(0.0,  uTexelSize)).x;
	heightUp *= height_scale;
    if(heightUp > max_height)
		heightUp = max_height;
	
	vec3 pos  = vec3(0 , Height, 0);
	vec3 posR = vec3(terrainSizeXZ/heightMapResoultion, heightRight, 0);
	vec3 posU = vec3(0, heightUp, terrainSizeXZ/heightMapResoultion);
	
	vec3 T = normalize(posR - pos);
	vec3 B = normalize(posU - pos);
	vec3 N = normalize(cross(B, T));
  
  if (dot(cross(N, T), B) < 0.0)
                T = T * -1.0;
  TBN = mat3(T,B,N);  
}
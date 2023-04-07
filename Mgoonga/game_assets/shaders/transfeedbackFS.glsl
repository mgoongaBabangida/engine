#version 410

uniform sampler2D ParticleTex;

in vec2 textureOffset1;
in vec2 textureOffset2;
in float blend;

layout ( location = 0 ) out vec4 FragColor;

void main()
{
	vec2 textureCoords = gl_PointCoord;
	textureCoords /= 4;	//NumRowsInTexture
    vec2 textureCoords1 = textureCoords + textureOffset1;
	vec2 textureCoords2 = textureCoords + textureOffset2;
    vec4 color1 = texture(ParticleTex,textureCoords1);
	vec4 color2 = texture(ParticleTex,textureCoords2);
	vec4 final_color = mix(color1,color2,blend);
	if(final_color.x<0.85 && final_color.y<0.85 && final_color.z<0.85 )
	  discard;
	else
	  final_color.a=0.85f;
	FragColor = final_color; //color1;
}

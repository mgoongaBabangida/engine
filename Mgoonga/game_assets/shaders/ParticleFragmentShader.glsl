#version 430

layout(binding=2) uniform sampler2D texture_diffuse1;

in vec2 textureCoords1;
in vec2 textureCoords2;
in float blend;

out vec4 out_Color;

void main()
{
	vec4 color1 = texture(texture_diffuse1,textureCoords1);
	vec4 color2 = texture(texture_diffuse1,textureCoords2);
	
	vec4 final_color = mix(color1, color2, blend);
	
	//if(final_color.x<0.85 && final_color.y<0.85 && final_color.z<0.85 )
	  //discard;
	//else
	  //final_color.a=0.85f;
	  
	out_Color = final_color; //color1;
}



#version 430

in vec4 ClipSpaceCoord;
in vec2 TextureCoords;
in vec3 ToCameraVector;
in vec3 FromLightVector;

out vec4 out_Color;


layout(binding=2) uniform sampler2D reflectionTexture;
layout(binding=3) uniform sampler2D refractionTexture;
layout(binding=4) uniform sampler2D normalTexture;
layout(binding=5) uniform sampler2D DUDV;

uniform float moveFactor;
uniform vec4 lightColor;

uniform float waveStrength = 2.2f;
uniform float shineDumper = 20.0f;
uniform float reflactivity = 0.6f;
uniform vec4  water_color = vec4(0.0f, 0.2f,0.4f,1.0f);
uniform float color_mix = 0.2f;
uniform float refrection_factor = 0.5f;
uniform float distortion_strength = 0.005f;

void main()
{
	vec2 ndc = (ClipSpaceCoord.xy/ClipSpaceCoord.w)/2.0 + 0.5;
	//vec2 refractTexCoords = vec2(ndc.x,ndc.y); 
	//vec2 reflectTexCoords = vec2(ndc.x,-ndc.y); 
	//vec2 refractTexCoords = TextureCoords; 
	//vec2 reflectTexCoords = TextureCoords; 

	int theight = textureSize(reflectionTexture,0).y;
	vec2 refractTexCoords = vec2(gl_FragCoord.x,gl_FragCoord.y);
	vec2 reflectTexCoords = vec2(gl_FragCoord.x,theight - gl_FragCoord.y);

	vec2 distortion1 = (texture(DUDV, vec2(TextureCoords.x + moveFactor,TextureCoords.y)).rg*2.0-1.0)* waveStrength; //* 0.1f;
	vec2 distortion2 = (texture(DUDV, vec2(-TextureCoords.x + moveFactor,TextureCoords.y + moveFactor)).rg*2.0-1.0)* waveStrength; //* 0.1f;
	vec2 totaldistortin = distortion1 + distortion2;

	//vec2 distortion = texture(DUDV, vec2(TextureCoords.x + moveFactor,TextureCoords.y)).rg*0,1;
	//distortion = TextureCoords + vec2(distortion.x,distortion.y+moveFactor);
	//vec2 totaldistortin = (texture(DUDV,distortion).rg*2.0-1.0)* waveStrength;

	refractTexCoords +=  totaldistortin;
	reflectTexCoords += totaldistortin;

	//refractTexCoords = clamp(refractTexCoords,0.001,0.999);
	//reflectTexCoords.x = clamp(reflectTexCoords.x,0.001,0.999);
	//reflectTexCoords.y = clamp(reflectTexCoords.y,-0.999,-0.001);

	//vec4 reflectColor = texture(reflectionTexture,  reflectTexCoords);
	//vec4 refractColor = texture(refractionTexture, refractTexCoords);

	vec4 reflectColor = texelFetch(reflectionTexture,  ivec2(reflectTexCoords),0);
	vec4 refractColor = texelFetch(refractionTexture, ivec2(refractTexCoords),0);

	vec4 normalMapColor = texture(normalTexture,totaldistortin * distortion_strength);
	vec3 normal = vec3(normalMapColor.r*2.0-1.0,normalMapColor.b,normalMapColor.g*2.0-1.0); 	
	normal = normalize(normal);

	float refrectionFactor = dot(normalize(ToCameraVector),vec3(0,1,0));
	refrectionFactor = pow(refrectionFactor, refrection_factor);

  vec3 Reflaction = normalize(reflect(FromLightVector,normal));
  float specular = max(dot(Reflaction,normalize(ToCameraVector) ) , 0.0f);
  specular = pow(specular, shineDumper);
  vec3 specularLight = lightColor.xyz  * specular * reflactivity;
  specularLight=clamp(specularLight, 0, 1);

	out_Color =  mix(reflectColor, refractColor, refrectionFactor);
	out_Color = mix(out_Color,water_color,color_mix) + vec4(specularLight,0.0f);
	//out_Color = normalMapColor;
};
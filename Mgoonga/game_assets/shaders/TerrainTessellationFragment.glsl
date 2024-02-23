#version 460 core

in float Height;
in vec2 texCoord;
in vec3 thePositionWorld;
in vec4 LocalSpacePos;

out vec4 FragColor;

struct Light 
{
    vec4 position;
    vec4 direction;
    
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

uniform Light light;

uniform float height_scale = 1.75f;

uniform vec4 eyePositionWorld;
uniform float shininess = 16.0f;

bool gamma_correction = true;
const int max_texture_array_size = 8;
const float epsilon = 0.001f;
uniform float min_height = 0.0f;
uniform float max_height = 1.75f;
uniform float base_start_heights[max_texture_array_size];
uniform int color_count = 4;
uniform float textureScale[max_texture_array_size];

layout(binding=4) uniform sampler2D normalMap;

layout(binding=12) uniform sampler2DArray  texture_array_albedo;

float inverseLerp(float a, float b, float value);
vec3 triplaner(float layer, vec3 blendAxes, vec4 localSpacePos);
vec3 SampleAlbedoTexture(vec3 Normal, vec4 localSpacePos);

void main()
{
	vec4 localSpacePos = vec4(LocalSpacePos.x, Height, LocalSpacePos.z, 1.0f); // ? /height_scale
    vec3 normal = texture(normalMap, texCoord).xyz;
    normal = normalize(normal * 2.0 - 1.0); 
	vec3 color  = SampleAlbedoTexture(normal, localSpacePos); //vec3(0.4f, 0.15f, 0.f);
	
	//diffuse	
    vec3 lightVector = -normalize(vec3(light.direction));
    float Brightness  = clamp(dot(lightVector, normal), 0, 1);
    vec3 diffuseColor = color * Brightness * light.diffuse.xyz;
	
	// specular shading
    vec3 eyeVector = normalize(eyePositionWorld.xyz - thePositionWorld);
	vec3 halfvector = normalize(eyeVector+lightVector);
	float spec = clamp(dot(normal, halfvector), 0, 1);	
    spec=pow(spec, shininess);
	   
	vec3 specularLight = light.specular.xyz * spec * color;
	specularLight = clamp(specularLight,0,1);
	
	vec3 ambientLight = light.ambient.xyz * color;
	
    FragColor = vec4(diffuseColor + specularLight + ambientLight, 1.0f);
}

float inverseLerp(float a, float b, float value)
{
	return clamp((value-a)/(b-a), 0.0f, 1.0f);
}

vec3 triplaner(float layer, vec3 blendAxes, vec4 localSpacePos)
{  
	int index = int(layer);
	vec3 scaledWorldPos = vec3(localSpacePos) / textureScale[index];
	if(gamma_correction)
	{
	  vec3 xProjection = vec3(pow(texture(texture_array_albedo, vec3(scaledWorldPos.yz, layer)).rgb, vec3(2.2f))) * abs(blendAxes.x);
	  vec3 yProjection = vec3(pow(texture(texture_array_albedo, vec3(scaledWorldPos.xz, layer)).rgb, vec3(2.2f))) * abs(blendAxes.y);
	  vec3 zProjection = vec3(pow(texture(texture_array_albedo, vec3(scaledWorldPos.xy, layer)).rgb, vec3(2.2f))) * abs(blendAxes.z);
	  return xProjection + yProjection + zProjection;
	}
	else
	{
		vec3 xProjection = vec3(texture(texture_array_albedo, vec3(scaledWorldPos.yz, layer))) * abs(blendAxes.x);
		vec3 yProjection = vec3(texture(texture_array_albedo, vec3(scaledWorldPos.xz, layer))) * abs(blendAxes.y);
		vec3 zProjection = vec3(texture(texture_array_albedo, vec3(scaledWorldPos.xy, layer))) * abs(blendAxes.z);
		return xProjection + yProjection + zProjection;
	}		
}

vec3 SampleAlbedoTexture(vec3 Normal, vec4 localSpacePos)
{
	vec3 colorAlbedo;
	float heightPercent = inverseLerp(min_height, max_height, Height);	
	vec3 blendAxes = normalize(Normal);
	
	for(int i = 0; i < color_count; ++i)
	{		
		if(heightPercent >= base_start_heights[i] && heightPercent <= base_start_heights[i+1])
		{
			vec3 colorMain = triplaner(i, blendAxes, localSpacePos);
			if(base_start_heights[i+1] - heightPercent < 0.05f)
			{
				float mixing = 1.0f / 0.05f;
				vec3 colorMix = triplaner(i+1, blendAxes, localSpacePos);
				colorAlbedo = mix(colorMix, colorMain, (base_start_heights[i+1] - heightPercent) * mixing);
			}
			else
				colorAlbedo = colorMain;
		}
	}
	return colorAlbedo;
}
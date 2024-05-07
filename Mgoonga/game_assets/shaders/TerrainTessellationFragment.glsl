#version 460 core

in float Height;
in vec2 texCoord;
in vec3 thePositionWorld;
in vec4 LocalSpacePos;
in mat3 TBN;

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

uniform Light lights[1]; // uniform size 

uniform vec4 eyePositionWorld;
uniform float shininess = 16.0f;

bool gamma_correction = true;
const int max_texture_array_size = 8;
const float epsilon = 0.001f;
uniform float min_height = 0.0f;
uniform float max_height = 1.75f;
uniform float base_start_heights[max_texture_array_size];
uniform int color_count = 4;
uniform int snow_color = 3;
uniform float snowness = 0.65f;
uniform float textureScale[max_texture_array_size];

uniform bool pbr_renderer = false;
uniform bool use_normal_texture_pbr = true;

layout(binding=4) uniform sampler2D normalMap;

layout(binding=12) uniform sampler2DArray  texture_array_albedo;
layout(binding=13) uniform sampler2DArray  texture_array_normal;
layout(binding=14) uniform sampler2DArray  texture_array_metallic;
layout(binding=15) uniform sampler2DArray  texture_array_roughness;
layout(binding=16) uniform sampler2DArray  texture_array_ao;

float inverseLerp(float a, float b, float value);
vec3 triplaner(float layer, vec3 blendAxes, vec4 localSpacePos);
vec3 SampleAlbedoTexture(vec3 Normal, vec4 localSpacePos);
vec3 SampleNormalTexture(vec3 Normal, vec4 localSpacePos);
float SampleMetallicTexture(vec3 Normal, vec4 localSpacePos);
float SampleRoughnessTexture(vec3 Normal, vec4 localSpacePos);
float SampleAOTexture(vec3 Normal, vec4 localSpacePos);
float ShouldBeSnow(vec3 Normal);

vec4 PhongModel();
vec4 PBRModel();
 
 const float PI = 3.14159265359;
  
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main()
{	
    FragColor = pbr_renderer ? PBRModel() : PhongModel();
}

vec4 PhongModel()
{
	vec4 localSpacePos = vec4(LocalSpacePos.x, Height, LocalSpacePos.z, 1.0f); // ? /height_scale
    vec3 normal = texture(normalMap, texCoord).xyz;
    normal = normalize(normal * 2.0 - 1.0); 
	vec3 color  = SampleAlbedoTexture(normal, localSpacePos); //vec3(0.4f, 0.15f, 0.f);
	
	//diffuse	
    vec3 lightVector = -normalize(vec3(lights[0].direction));
    float Brightness  = clamp(dot(lightVector, normal), 0, 1);
    vec3 diffuseColor = color * Brightness * lights[0].diffuse.xyz;
	
	// specular shading
    vec3 eyeVector = normalize(eyePositionWorld.xyz - thePositionWorld);
	vec3 halfvector = normalize(eyeVector+lightVector);
	float spec = clamp(dot(normal, halfvector), 0, 1);	
    spec=pow(spec, shininess);
	   
	vec3 specularLight = lights[0].specular.xyz * spec * color;
	specularLight = clamp(specularLight,0,1);
	
	vec3 ambientLight = lights[0].ambient.xyz * color;
	
    return vec4(diffuseColor + specularLight + ambientLight, 1.0f);
}

vec4 PBRModel()
{
	vec4 localSpacePos = vec4(LocalSpacePos.x, Height, LocalSpacePos.z, 1.0f);
    vec3 baseNormal 	= texture(normalMap, texCoord).xyz;
    baseNormal = normalize(baseNormal * 2.0 - 1.0);
	
	vec3 N;
	if(use_normal_texture_pbr)
	{
		vec3 normal 	= SampleNormalTexture(baseNormal, localSpacePos);
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(TBN * normal);
		N = normalize(normal);
	}
	else
	{
		N = baseNormal;
	}
	
	vec3 albedo     = SampleAlbedoTexture(baseNormal, localSpacePos);
    float metallic  = SampleMetallicTexture(baseNormal, localSpacePos);
    float roughness = SampleRoughnessTexture(baseNormal, localSpacePos);
    float ao        = SampleAOTexture(baseNormal, localSpacePos);
	
    vec3 V = normalize(eyePositionWorld.xyz - thePositionWorld);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 1; ++i) 
    {
        // calculate per-light radiance
        vec3 L = -normalize(vec3(lights[i].direction)); // directional light
		// normalize(lights[i].position.xyz - thePositionWorld);
        vec3 H = normalize(V + L);
        float distance    = length(lights[i].position.xyz - thePositionWorld);
        float attenuation = 1.0; // / (distance * distance);
        vec3 radiance     = lights[i].ambient.xyz * 15 * attenuation;       // light color  
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }   
  
    vec3 ambient = vec3(0.15) * albedo * ao;
    vec3 color = ambient + Lo;
	
    //color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.2));  
   
    return vec4(color, 1.0);
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
			vec3 colorMix = triplaner(i-1, blendAxes, localSpacePos);
			if(i == snow_color)
			{
				float should_be_snow = ShouldBeSnow(Normal);
				if(should_be_snow > snowness)
					colorAlbedo = colorMain;
				else if(should_be_snow < snowness - 0.05)
					colorAlbedo = colorMix;
				else
				{
					float mixing = 1.0f / 0.05f;
					colorAlbedo = mix(colorMain, colorMix,(snowness - should_be_snow) * mixing);
				}				
			}
			else if(base_start_heights[i+1] - heightPercent < 0.05f && (i != snow_color-1 || snowness < 0.60) )
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

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 SampleNormalTexture(vec3 Normal, vec4 localSpacePos)
{
	vec3 outNormal = vec3(0,1,0);
	float heightPercent = inverseLerp(min_height, max_height, Height);
	vec3 blendAxes = normalize(Normal);	
	for(int i = 0; i < color_count; ++i)
	{		
		if(heightPercent >= base_start_heights[i] && heightPercent <= base_start_heights[i+1])
		{
			int index = int(i);
			vec3 scaledWorldPos = vec3(localSpacePos) / textureScale[index];
			vec3 xProjection = vec3(texture(texture_array_normal, vec3(scaledWorldPos.yz, index))) * abs(blendAxes.x);
			vec3 yProjection = vec3(texture(texture_array_normal, vec3(scaledWorldPos.xz, index))) * abs(blendAxes.y);
			vec3 zProjection = vec3(texture(texture_array_normal, vec3(scaledWorldPos.xy, index))) * abs(blendAxes.z);
			outNormal = xProjection + yProjection + zProjection;
		}
	}
	return outNormal;
}

float SampleMetallicTexture(vec3 Normal, vec4 localSpacePos)
{
	float metallic = 0.0f;
	return metallic;
	float heightPercent = inverseLerp(min_height, max_height, Height);
	vec3 blendAxes = normalize(Normal);	
	for(int i = 0; i < color_count; ++i)
	{		
		if(heightPercent >= base_start_heights[i] && heightPercent <= base_start_heights[i+1])
		{
			int index = int(i);
			vec3 scaledWorldPos = vec3(localSpacePos) / textureScale[index];
			//float xProjection = texture(texture_array_metallic, vec3(scaledWorldPos.yz, index)).r * abs(blendAxes.x);
			float yProjection = texture(texture_array_metallic, vec3(scaledWorldPos.xz, index)).r; // * abs(blendAxes.y);
			//float zProjection = texture(texture_array_metallic, vec3(scaledWorldPos.xy, index)).r * abs(blendAxes.z);
			metallic = yProjection; //(xProjection + yProjection + zProjection)/3.f;
		}
	}
	return clamp(metallic, 0, 1);
}

float SampleRoughnessTexture(vec3 Normal, vec4 localSpacePos)
{
	float roughness = 0.9f;
	return roughness;
	float heightPercent = inverseLerp(min_height, max_height, Height);
	vec3 blendAxes = normalize(Normal);	
	for(int i = 0; i < color_count; ++i)
	{		
		if(heightPercent >= base_start_heights[i] && heightPercent <= base_start_heights[i+1])
		{
			int index = int(i);
			vec3 scaledWorldPos = vec3(localSpacePos) / textureScale[index];
			//float xProjection = texture(texture_array_roughness, vec3(scaledWorldPos.yz, index)).r * abs(blendAxes.x);
			float yProjection = texture(texture_array_roughness, vec3(scaledWorldPos.xz, index)).r; // * abs(blendAxes.y);
			//float zProjection = texture(texture_array_roughness, vec3(scaledWorldPos.xy, index)).r * abs(blendAxes.z);
			roughness = yProjection; //(xProjection + yProjection + zProjection)/3.f;
		}
	}
	return clamp(roughness, 0, 1);
}

float SampleAOTexture(vec3 Normal, vec4 localSpacePos)
{
	float AO = 1.0f;
	return AO;
	float heightPercent = inverseLerp(min_height, max_height, Height);
	vec3 blendAxes = normalize(Normal);	
	for(int i = 0; i < color_count; ++i)
	{		
		if(heightPercent >= base_start_heights[i] && heightPercent <= base_start_heights[i+1])
		{
			int index = int(i);
			vec3 scaledWorldPos = vec3(localSpacePos) / textureScale[index];
			//float xProjection = texture(texture_array_ao, vec3(scaledWorldPos.yz, index)).r * abs(blendAxes.x);
			float yProjection = texture(texture_array_ao, vec3(scaledWorldPos.xz, index)).r; // * abs(blendAxes.y);
			//float zProjection = texture(texture_array_ao, vec3(scaledWorldPos.xy, index)).r * abs(blendAxes.z);
			AO = yProjection; //(xProjection + yProjection + zProjection)/3.f;
		}
	}
	return clamp(AO, 0, 1);
}

float ShouldBeSnow(vec3 Normal)
{
	const int numLights = 3;  // Number of light sources
	vec3 lightDirs[numLights];
	lightDirs[0] = normalize(vec3(-1, 1, -1));
	lightDirs[1] = vec3(0, 1, 0);
	lightDirs[2] = normalize(vec3(1, 1, 1));
	
 // Initialize light intensity
    float totalLightIntensity = 0.0;

    // Calculate total light intensity by summing contributions from each light source
    for (int i = 0; i < numLights; ++i) {
        totalLightIntensity += dot(normalize(Normal), normalize(lightDirs[i]));
    }
    
    // Normalize total light intensity
    totalLightIntensity /= float(numLights);
    
    // Calculate the dot product between the surface normal and the view direction
	//vec3 viewDir = normalize(eyePositionWorld.xyz - thePositionWorld);
    //float viewIntensity = dot(normalize(Normal), normalize(viewDir));
	
	return clamp(totalLightIntensity, 0, 1);

}
	
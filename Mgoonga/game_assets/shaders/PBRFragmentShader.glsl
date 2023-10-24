#version 430 core

out vec4 FragColor;

in vec2 Texcoord;
in vec3 thePosition; //WorldPos
in vec3 theNormal;   //theNormal
in mat3 TBN;

// material parameters
uniform vec4  albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;
uniform bool textured = true;

layout(binding=2) uniform sampler2D albedoMap;
layout(binding=3) uniform sampler2D metallicMap;
layout(binding=4) uniform sampler2D normalMap;
layout(binding=5) uniform sampler2D roughnessMap;
layout(binding=6) uniform sampler2D texture_emissionl;

// IBL
layout(binding=9)  uniform samplerCube irradianceMap;
layout(binding=10) uniform samplerCube prefilterMap;
layout(binding=11) uniform sampler2D   brdfLUT;
 
// lights
uniform vec4 lightPositions[1];
uniform vec4 lightColors[1];

uniform vec4 camPos;
uniform bool gamma_correction = true;
uniform float emission_strength = 1.0f;

const float PI = 3.14159265359;
  
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

void main()
{	
   vec3 albedo_f;
   vec3 theNormal_f;
   float metallic_f;
   float roughness_f;
   
   if(textured)	
   {
   if(gamma_correction)
     albedo_f    = pow(texture(albedoMap, Texcoord).rgb, vec3(2.2));
   else
	 albedo_f    = texture(albedoMap, Texcoord).rgb;
	 
     theNormal_f = texture(normalMap, Texcoord).rgb;
	 // Transform normal vector to range [-1,1]
	 theNormal_f = normalize(theNormal_f * 2.0 - 1.0);
	 theNormal_f = normalize(TBN * theNormal_f);
     metallic_f  = texture(metallicMap, Texcoord).r;
     roughness_f = texture(roughnessMap, Texcoord).r;
   }
   else
   {
	albedo_f = albedo.xyz;
	theNormal_f = theNormal;
	metallic_f = metallic;
	roughness_f = roughness;
   }
   
    vec3 N = normalize(theNormal_f);
    vec3 V = normalize(camPos.xyz - thePosition);
	vec3 R = reflect(-V, N);
	
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo_f, metallic_f);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 1; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i].xyz - thePosition);
        vec3 H = normalize(V + L);
        float distance    = length(lightPositions[i].xyz - thePosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = lightColors[i].xyz * attenuation;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness_f);        
        float G   = GeometrySmith(N, V, L, roughness_f);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
		vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001); 
		
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic_f;	   
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo_f / PI + specular) * radiance * NdotL; 
    }

	// ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness_f);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic_f;
	
	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse      = irradiance * albedo_f;
	
	// sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness_f * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness_f)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	
	vec3 ambient = (kD * diffuse + specular) * ao;
	
    vec3 color = ambient + Lo;
	
	vec3 emissive_color;
	  if(gamma_correction)
		emissive_color = vec3(pow(texture(texture_emissionl, Texcoord).rgb, vec3(2.2f)));
	  else
		emissive_color = vec3(texture(texture_emissionl, Texcoord));
	color.rgb += (emissive_color * emission_strength);
	
    FragColor = vec4(color, 1.0);
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
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
} 

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 
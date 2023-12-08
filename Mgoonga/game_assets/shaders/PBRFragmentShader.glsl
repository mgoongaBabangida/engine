#version 430 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out float mask;

in vec2 Texcoord;
in vec3 thePosition; //WorldPos
in vec3 theNormal;   //theNormal
in mat3 TBN;
in vec4 LightSpacePos; //shadows

// material parameters
uniform vec4  albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform bool textured = true;
uniform bool use_metalic_texture = true;
uniform bool use_normalmap_texture = true;
uniform bool use_roughness_texture = true;

uniform bool ibl_on = true;

layout(binding=0) uniform samplerCube 	   depth_cube_map;// Shadow point
layout(binding=1) uniform sampler2D	   	   depth_texture; // Shadow dir

layout(binding=2) uniform sampler2D albedoMap;
layout(binding=3) uniform sampler2D metallicMap;
layout(binding=4) uniform sampler2D normalMap;
layout(binding=5) uniform sampler2D roughnessMap;
layout(binding=6) uniform sampler2D texture_emissionl;

// IBL
layout(binding=9)  uniform samplerCube irradianceMap;
layout(binding=10) uniform samplerCube prefilterMap;
layout(binding=11) uniform sampler2D   brdfLUT;

layout(binding=13) uniform sampler2DArray  texture_array_csm;

// lights - > to structs ///////////////
uniform vec4 lightPositions[1];
uniform vec4 lightDirections[1];
uniform vec4 lightColors[1];

uniform float constant[1];
uniform float linear[1];
uniform float quadratic[1];

uniform float cutOff[1];
uniform float outerCutOff[1];

uniform bool flash[1];
///////////////////////////////////////

struct FogInfo
{
  float maxDist;
  float minDist;
  vec4 color;
	bool fog_on;
	float density;
	float gradient;
};
uniform FogInfo Fog;

uniform vec4 camPos;
uniform bool gamma_correction = true;
uniform float emission_strength = 1.0f;

const float PI = 3.14159265359;
  
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

uniform bool shadow_directional = true;
uniform bool use_csm_shadows = false;
//csm
uniform mat4 view;
uniform float far_plane;
uniform float farPlane;
uniform int cascadeCount;
uniform float[10] cascadePlaneDistances;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal);
float ShadowCalculationCubeMap(vec3 fragPos);
float ShadowCalculationCSM(vec4 fragWorldPos, vec3 lightDir, vec3 normal);

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
   }
   else
	  albedo_f = albedo.xyz;
   
   if(use_metalic_texture)
    metallic_f  = texture(metallicMap, Texcoord).r;
   else
    metallic_f = metallic;

    if(use_normalmap_texture)
    {
      theNormal_f = texture(normalMap, Texcoord).rgb;
	    // Transform normal vector to range [-1,1]
	    theNormal_f = normalize(theNormal_f * 2.0 - 1.0);
	    theNormal_f = normalize(TBN * theNormal_f);
    }
    else
      theNormal_f = theNormal;

   if(use_roughness_texture)
    roughness_f  =  texture(roughnessMap, Texcoord).r;
   else
    roughness_f = roughness;

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
      float attenuation = 1.0f /(constant[i] + linear[i] * distance + quadratic[i] * (distance * distance));
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
      vec3 outgoing_radiance = (kD * albedo_f / PI + specular) * radiance * NdotL;
      if(flash[i])
      {
       float theta     = dot(L, normalize(-lightDirections[i].xyz));
	     float epsilon   = cutOff[i] - outerCutOff[i];
	     float intensity = clamp((theta - outerCutOff[i]) / epsilon, 0.0, 1.0);
       outgoing_radiance *= intensity;
      }
      Lo += outgoing_radiance;
    }
	
  vec3 color = Lo;
  if(ibl_on)
  {
  	// ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness_f);
	  vec3 kS = F;
	  vec3 kD = 1.0 - kS;
	  kD *= 1.0 - metallic_f;

  	vec3 irradiance = texture(irradianceMap, N).rgb;
	  vec3 diffuse    = irradiance * albedo_f;

	  // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness_f * MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness_f)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	  
	  vec3 ambient = (kD * diffuse + specular) * ao;
    color = color + ambient;
  }
  else
  {
   vec3 ambient = vec3(0.05) * albedo_f * ao; //@ 0.03 or other ?
   color = color + ambient;
  }
	
	vec3 emissive_color;
		emissive_color = vec3(texture(texture_emissionl, Texcoord));
	color.rgb += (emissive_color * emission_strength);
	
 float shadow;
 vec3 lightVector = -normalize(vec3(lightDirections[0]));	 // @should be for every light
  if(shadow_directional && !use_csm_shadows)
	 shadow =  ShadowCalculation(LightSpacePos, lightVector, N);
  else if(shadow_directional && use_csm_shadows)
	 shadow = ShadowCalculationCSM(vec4(thePosition, 1.0f), lightVector, N);
  else
	 shadow =  ShadowCalculationCubeMap(thePosition);
	
  FragColor = vec4(color * shadow, 1.0);

  if(Fog.fog_on)
	{
	 float dist = abs(vec4(view * vec4(thePosition, 1.0f)).z);
	 float fogFactor = exp(-pow(dist * Fog.density, Fog.gradient));
	 fogFactor = clamp( fogFactor, 0.0f, 1.0f );
	 FragColor.rgb = mix(Fog.color.rgb, FragColor.rgb, fogFactor);
	}

  mask = 1.0f - roughness_f;
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

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal )
{
  //perform perspective divide
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  projCoords = projCoords * 0.5 + 0.5;
  float closestDepth = texture(depth_texture, projCoords.xy).r;
  float currentDepth = projCoords.z;
  float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
  
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(depth_texture, 0);
  for(int x = -2; x <= 2; ++x)
  {
     for(int y = -2; y <= 2; ++y)
     {
         float pcfDepth = texture(depth_texture, projCoords.xy + vec2(x, y) * texelSize).r; 
         shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
     }    
  }
  shadow /= 25.0;
  
  if(fragPosLightSpace.z > 1.0) //far plane issue!
     shadow = 0.0;
  return clamp((0.5f + (1.0f - shadow)), 0.0f, 1.0f);
} 

float ShadowCalculationCubeMap(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPositions[0].xyz;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(depth_cube_map, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return clamp((0.5f + (1.0f - shadow)), 0.0f, 1.0f);
} 

float ShadowCalculationCSM(vec4 fragPosWorldSpace, vec3 lightDir, vec3 normal)
{
	vec4 fragPosViewSpace = view * fragPosWorldSpace;
	float depthValue = abs(fragPosViewSpace.z);		
	int layer = -1;
	for (int i = 0; i < cascadeCount; ++i)
	{
		if (depthValue < cascadePlaneDistances[i])
		{
			layer = i;
			break;
		}
	}
	if (layer == -1)
	{
		layer = cascadeCount;
	}		
	vec4 fragPosLightSpace = lightSpaceMatrices[layer] * fragPosWorldSpace;
	
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
		
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	if (currentDepth  > 1.0)
	{
		return 1.0;
	}
	// calculate bias (based on depth map resolution and slope)
	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
	if (layer == cascadeCount)
	{
		bias *= 1 / (farPlane * 0.5f);
	}
	else
	{
		bias *= 1 / (cascadePlaneDistances[layer] * 0.5f);
	}
	
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / vec2(textureSize(texture_array_csm, 0));
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(
						texture_array_csm,
						vec3(projCoords.xy + vec2(x, y) * texelSize,
						layer)
						).r; 
			shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
		
	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}			
	return clamp((0.5f + (1.0f - shadow)), 0.0f, 1.0f);
} 
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

//Area lights
layout(binding=7) uniform sampler2D LTC1; // for inverse M
layout(binding=8) uniform sampler2D LTC2; // GGX norm, fresnel, 0(unused), sphere

// IBL
layout(binding=9)  uniform samplerCube irradianceMap;
layout(binding=10) uniform samplerCube prefilterMap;
layout(binding=11) uniform sampler2D   brdfLUT;

layout(binding=13) uniform sampler2DArray  texture_array_csm;

const float LUT_SIZE  = 64.0; // ltc_texture size
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;

const int max_lights = 2;
// lights - > to structs ///////////////

uniform int num_lights = 2;

uniform vec4 lightPositions[max_lights];
uniform vec4 lightDirections[max_lights];
uniform vec4 lightColors[max_lights];

uniform float constant[max_lights];
uniform float linear[max_lights];
uniform float quadratic[max_lights];

uniform float cutOff[max_lights];
uniform float outerCutOff[max_lights];

uniform bool flash[max_lights];
uniform bool isAreaLight[max_lights];

uniform float intensity[max_lights];
uniform mat4  points[max_lights];
uniform bool  twoSided[max_lights];
uniform float radius[max_lights];
///////////////////////////////////////

struct AreaLight
{
    float intensity;
    vec4 color;
    vec4 points[4];
    bool twoSided;
};

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

//area lights functions
// Vector form without project to the plane (dot with the normal)
// Use for proxy sphere clipping
vec3 IntegrateEdgeVec(vec3 v1, vec3 v2);
float IntegrateEdge(vec3 v1, vec3 v2);
// P is fragPos in world space (LTC distribution)
vec3 LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, vec3 points[4], bool twoSided);
// PBR-maps for roughness (and metallic) are usually stored in non-linear
// color space (sRGB), so we use these functions to convert into linear RGB.
vec3 PowVec3(vec3 v, float p);
const float gamma = 2.2;
vec3 ToLinear(vec3 v) { return PowVec3(v, gamma); }
vec3 ToSRGB(vec3 v)   { return PowVec3(v, 1.0/gamma); }
vec4 CalculateAreaLight(AreaLight _areaLight, vec4 _areaLightTranslate, vec3 _albedo, vec3 _normal, float _roughness);

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

bool isAnyNan(vec3 v) { return isnan(v.x) || isnan(v.y) || isnan(v.z);}

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
	  theNormal_f = TBN * theNormal_f;
	  if(isAnyNan(theNormal_f))
		theNormal_f = theNormal;
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
    for(int i = 0; i < num_lights; ++i)
    {
	  float distance    = length(lightPositions[i].xyz - thePosition);
	  if(isAreaLight[i])
	  {
		if(distance < radius[i])
		{
		  AreaLight areaLight;
		  areaLight.points[0] = points[i][0];
		  areaLight.points[1] = points[i][1];
		  areaLight.points[2] = points[i][2];
		  areaLight.points[3] = points[i][3];
		  areaLight.intensity = intensity[i];
		  areaLight.twoSided = twoSided[i];
		  areaLight.color = lightColors[i];
		  Lo += vec3(CalculateAreaLight(areaLight, lightPositions[i], albedo_f, N, roughness_f));
		}
	  }
	  else
	  {
		// calculate per-light radiance
		vec3 L = normalize(lightPositions[i].xyz - thePosition);
		vec3 H = normalize(V + L);
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

vec3 IntegrateEdgeVec(vec3 v1, vec3 v2)
{
    // Using built-in acos() function will result flaws
    // Using fitting result for calculating acos()
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

float IntegrateEdge(vec3 v1, vec3 v2)
{
    return IntegrateEdgeVec(v1, v2).z;
}

vec3 LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, vec3 points[4], bool twoSided)
{
    // construct orthonormal basis around N
    vec3 T1, T2;
    T1 = normalize(V - N * dot(V, N));
    T2 = cross(N, T1);

    // rotate area light in (T1, T2, N) basis
    Minv = Minv * transpose(mat3(T1, T2, N));

    // polygon (allocate 4 vertices for clipping)
    vec3 L[4];
    // transform polygon from LTC back to origin Do (cosine weighted)
    L[0] = Minv * (points[0].xyz - P);
    L[1] = Minv * (points[1].xyz - P);
    L[2] = Minv * (points[2].xyz - P);
    L[3] = Minv * (points[3].xyz - P);

    // use tabulated horizon-clipped sphere
    // check if the shading point is behind the light
    vec3 dir = points[0].xyz - P; // LTC space
    vec3 lightNormal = cross(points[1].xyz - points[0].xyz, points[3].xyz - points[0].xyz);
    bool behind = (dot(dir, lightNormal) < 0.0);

    // cos weighted space
    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);

    // integrate
    vec3 vsum = vec3(0.0);
    vsum += IntegrateEdgeVec(L[0], L[1]);
    vsum += IntegrateEdgeVec(L[1], L[2]);
    vsum += IntegrateEdgeVec(L[2], L[3]);
    vsum += IntegrateEdgeVec(L[3], L[0]);

    // form factor of the polygon in direction vsum
    float len = length(vsum);

    float z = vsum.z/len;
    if (behind)
        z = -z;

    vec2 uv = vec2(z*0.5f + 0.5f, len); // range [0, 1]
    uv = uv*LUT_SCALE + LUT_BIAS;

    // Fetch the form factor for horizon clipping
    float scale = texture(LTC2, uv).w;

    float sum = len*scale;
    if (!behind && !twoSided)
        sum = 0.0;

    // Outgoing radiance (solid angle) for the entire polygon
    vec3 Lo_i = vec3(sum, sum, sum);
    return Lo_i;
}

vec3 PowVec3(vec3 v, float p)
{
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

vec4 CalculateAreaLight(AreaLight _areaLight, vec4 _areaLightTranslate, vec3 _albedo, vec3 _normal, float _roughness)
{		
    // gamma correction
    //vec3 mDiffuse = texture(material.diffuse, Texcoord).xyz;// * vec3(0.7f, 0.8f, 0.96f);
	
	vec3 mDiffuse = _albedo;
    vec3 mSpecular = ToLinear(vec3(0.23f, 0.23f, 0.23f)); // mDiffuse
    vec3 result = vec3(0.0f);

    vec3 N = normalize(_normal);
    vec3 V = normalize(camPos.xyz - thePosition);
    vec3 P = thePosition;
    float dotNV = clamp(dot(N, V), 0.0f, 1.0f);

    // use roughness and sqrt(1-cos_theta) to sample M_texture
    vec2 uv = vec2(clamp(_roughness, 0.01f, 1.0f), sqrt(1.0f - dotNV));
    uv = uv*LUT_SCALE + LUT_BIAS;

	uv.x = clamp(uv.x, 0.0f, 1.0f);
	uv.y = clamp(uv.y, 0.0f, 1.0f);

    // get 4 parameters for inverse_M
    vec4 t1 = texture(LTC1, uv);

    // Get 2 parameters for Fresnel calculation
    vec4 t2 = texture(LTC2, uv);

    mat3 Minv = mat3(
        vec3(t1.x, 0, t1.y),
        vec3(  0,  1,    0),
        vec3(t1.z, 0, t1.w)
    );

    // translate light source for testing
    vec3 translatedPoints[4];
    translatedPoints[0] = _areaLight.points[0].xyz + _areaLightTranslate.xyz;
    translatedPoints[1] = _areaLight.points[1].xyz + _areaLightTranslate.xyz;
    translatedPoints[2] = _areaLight.points[2].xyz + _areaLightTranslate.xyz;
    translatedPoints[3] = _areaLight.points[3].xyz + _areaLightTranslate.xyz;

    // Evaluate LTC shading
    vec3 diffuse = LTC_Evaluate(N, V, P, mat3(1), translatedPoints, _areaLight.twoSided);
    vec3 specular = LTC_Evaluate(N, V, P, Minv, translatedPoints, _areaLight.twoSided);

    // GGX BRDF shadowing and Fresnel
    // t2.x: shadowedF90 (F90 normally it should be 1.0)
    // t2.y: Smith function for Geometric Attenuation Term, it is dot(V or L, H).
    specular *= mSpecular*t2.x + (1.0f - mSpecular) * t2.y;

    result = _areaLight.color.xyz * _areaLight.intensity * (specular + mDiffuse * diffuse);

    return vec4(ToSRGB(result), 1.0f);
}

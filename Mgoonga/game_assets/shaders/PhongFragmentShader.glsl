#version 430

out vec4 outColor;

struct Material {
    vec3  albedo;
    float metallic;
    float roughness;    
    float ao;
}; 

struct Light 
{
    vec4 position;
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

in vec3 theColor;
in vec3 theNormal;
in vec3 thePosition;
in vec2 Texcoord;
in vec4 LightSpacePos;
in mat3 TBN;
in vec4 debug;

uniform Material material;
uniform Light light;

subroutine vec3 LightingPtr(Light light, vec3 normal, vec3 thePosition, vec2 TexCoords);
subroutine uniform LightingPtr LightingFunction;

layout(binding=0) uniform samplerCube 	   depth_cube_map;// Shadow point
layout(binding=1) uniform sampler2D		   depth_texture; // Shadow dir

layout(binding=2) uniform sampler2D        texture_diffuse1;
layout(binding=3) uniform sampler2D        texture_specular1;
layout(binding=4) uniform sampler2D        texture_normal1;
layout(binding=5) uniform sampler2D        texture_depth1;
layout(binding=6) uniform sampler2D        texture_emissionl;
layout(binding=7) uniform sampler2D        texture_ssao;

uniform vec3 eyePositionWorld;
uniform bool normalMapping = true;
uniform bool shadow_directional = true;

uniform float far_plane;
uniform float shininess = 32.0f;
uniform float ssao_threshold = 0.9f;

uniform bool gamma_correction = true;
uniform bool debug_white_color = false;
uniform bool debug_white_texcoords = false;

uniform bool tone_mapping = true;
uniform float hdr_exposure = 1.0f;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal);
float ShadowCalculationCubeMap(vec3 fragPos);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

subroutine(LightingPtr) vec3 calculatePhongPointSpecDif(Light light, vec3 normal, vec3 thePosition, vec2 TexCoords)
{
  //Diffuse
  vec3 lightVector  = normalize(vec3(light.position)-thePosition);
  float Brightness  = clamp(dot(lightVector, normal), 0, 1);
  vec3 diffuseLight;
  if(gamma_correction)
	diffuseLight = vec3(light.diffuse * Brightness * pow(texture(texture_diffuse1, TexCoords).rgb, vec3(2.2f)));
  else
	diffuseLight = vec3(light.diffuse * Brightness * vec3(texture(texture_diffuse1, TexCoords))); //Brightness * material.diffuse
  
  //Specular
  vec3 Reflaction = reflect(-lightVector,normal);
  vec3 eyeVector  = normalize(eyePositionWorld-thePosition); 
  float spec      = clamp(dot(Reflaction,eyeVector), 0, 1);

  spec = pow(spec, shininess);
  vec3 specularLight = vec3(light.specular *spec * vec3(texture(texture_specular1, TexCoords)));//* material.specular
  specularLight=clamp(specularLight,0,1);

  // Attenuation
    float distance    = length(vec3(light.position) - thePosition);
    float attenuation = 1.0f /(light.constant + light.linear * distance + light.quadratic * (distance * distance));    
 
    diffuseLight  *= attenuation;
    specularLight *= attenuation;

    return diffuseLight + specularLight;
}

subroutine(LightingPtr) vec3 calculatePhongDirectionalSpecDif(Light light, vec3 normal, vec3 thePosition, vec2 TexCoords)
{
    vec3 lightVector = -normalize(vec3(light.direction));
    // diffuse shadingfloat 
	float Brightness  = clamp(dot(lightVector, normal), 0, 1);
	vec3 diffuseLight;
	if(gamma_correction)
		diffuseLight = vec3(light.diffuse * Brightness * pow(texture(texture_diffuse1, TexCoords).rgb, vec3(2.2f)));
	else
		diffuseLight  = vec3(light.diffuse  * Brightness * vec3(texture(texture_diffuse1, TexCoords)));
	
   // specular shading
    vec3 Reflaction = reflect(-lightVector, normal);
    vec3 eyeVector = normalize(eyePositionWorld - thePosition);
	float spec      = clamp(dot(Reflaction,eyeVector), 0, 1);	
    
	spec = pow(spec, shininess);   
	vec3 specularLight = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
	specularLight=clamp(specularLight,0,1);
	
    return diffuseLight + specularLight;
}

subroutine(LightingPtr) vec3 calculatePhongFlashSpecDif(Light light, vec3 normal, vec3 thePosition, vec2 TexCoords)
{
	vec3 lightDir= normalize(vec3(light.position)-thePosition);
	float theta     = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	if(theta > light.cutOff) 
	{
		vec3 ret = calculatePhongPointSpecDif(light, normal, thePosition, TexCoords);
		return ret *= intensity;
	}
	else
	 return vec3(0.0f,0.0f,0.0f);
}

subroutine(LightingPtr) vec3 calculateBlinnPhongPointSpecDif (Light light, vec3 normal, vec3 thePosition, vec2 TexCoords)
{
  //Diffuse
  vec3 lightVector  = normalize(vec3(light.position)-thePosition);
  float Brightness  = clamp(dot(lightVector, normal),0,1);
  vec3 diffuseLight;
	if(gamma_correction)
		diffuseLight = vec3(light.diffuse * Brightness * pow(texture(texture_diffuse1, TexCoords).rgb, vec3(2.2f)));
	else
		diffuseLight = vec3(light.diffuse * Brightness * vec3(texture(texture_diffuse1, TexCoords)));//Brightness*material.diffuse
  
  //Specular
  vec3 eyeVector= normalize(eyePositionWorld-thePosition); 
  vec3 halfvector = normalize(eyeVector+lightVector);
  float spec=clamp(dot(normal, halfvector), 0, 1);
  spec=pow(spec, shininess);
  vec3 specularLight=vec3(light.specular *spec * vec3(texture(texture_specular1, TexCoords)));//* material.specular
  specularLight=clamp(specularLight,0,1);

  // Attenuation
    float distance    = length(vec3(light.position) - thePosition);
    float attenuation = 1.0f /(light.constant + light.linear * distance + light.quadratic * (distance * distance));    
 
    diffuseLight  *= attenuation;
    specularLight *= attenuation;

    return diffuseLight + specularLight;
}

subroutine(LightingPtr) vec3 calculateBlinnPhongDirectionalSpecDif(Light light, vec3 normal, vec3 thePosition, vec2 TexCoords)
{
    vec3 lightVector = -normalize(vec3(light.direction));
    // diffuse shadingfloat 
	float Brightness  = clamp(dot(lightVector, normal), 0, 1);
	vec3 diffuseLight;
	if(gamma_correction)
		diffuseLight = vec3(light.diffuse * Brightness * pow(texture(texture_diffuse1, TexCoords).rgb, vec3(2.2f)));
	else
		diffuseLight  = vec3(light.diffuse  * Brightness * vec3(texture(texture_diffuse1, TexCoords)));
	
   //Specular
    vec3 eyeVector= normalize(eyePositionWorld-thePosition); 
    vec3 halfvector = normalize(eyeVector+lightVector);
    float spec = clamp(dot(normal, halfvector), 0, 1);
    spec=pow(spec, shininess);
    vec3 specularLight = vec3(light.specular * spec * vec3(texture(texture_specular1, TexCoords)));//* material.specular
    specularLight=clamp(specularLight,0,1);	
	
    return diffuseLight + specularLight;
}

subroutine(LightingPtr) vec3 calculateBlinnPhongFlashSpecDif(Light light, vec3 normal, vec3 thePosition, vec2 TexCoords)
{
	vec3 lightDir= normalize(vec3(light.position)-thePosition);
	float theta     = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	if(theta > light.cutOff) 
	{
		vec3 ret = calculateBlinnPhongPointSpecDif(light, normal, thePosition, TexCoords);
		return ret *= intensity;
	}
	else
	 return vec3(0.0f,0.0f,0.0f);
}

void main()
{   
	// Paralax mapping
	//vec3 tangetnViewDir   = normalize(TBN * eyePositionWorld - TBN * thePosition);
	//vec2 fTexCoords = ParallaxMapping(Texcoord,  tangetnViewDir);
	//if(fTexCoords.x > 1.0 || fTexCoords.y > 1.0 || fTexCoords.x < 0.0 || fTexCoords.y < 0.0)
		//discard;
	
	vec3 bNormal;
	if(normalMapping)
	{
		// Obtain normal from normal map in range [0,1]
		bNormal = texture(texture_normal1, Texcoord).rgb;
		// Transform normal vector to range [-1,1]
		bNormal = normalize(bNormal * 2.0 - 1.0);   
		bNormal = normalize(TBN * bNormal);
	}
	else
		bNormal = theNormal;

  //Ambient
  vec2 buf_relative_tex_coord = vec2(gl_FragCoord[0]/ 1200.0f, gl_FragCoord[1] / 600.0f);
  float AmbientOcclusion = 1.0f;
  
  if(gamma_correction)
	AmbientOcclusion = texture(texture_ssao, buf_relative_tex_coord).r;
  else
	AmbientOcclusion = texture(texture_ssao, buf_relative_tex_coord).r;
  
  if(AmbientOcclusion < ssao_threshold)
	AmbientOcclusion = 0.0f;
	
  vec3 dif_texture;
	if(gamma_correction)
		dif_texture = vec3(pow(texture(texture_diffuse1, Texcoord).rgb, vec3(2.2f)));
	else
		dif_texture = vec3(texture(texture_diffuse1, Texcoord));
		
  vec3 ambientLight = light.ambient * dif_texture * AmbientOcclusion; 

     float shadow;
	 vec3 lightVector = -normalize(vec3(light.direction));	 
     if(shadow_directional)
		shadow =  ShadowCalculation(LightSpacePos, lightVector, bNormal);
     else
		shadow =  ShadowCalculationCubeMap(thePosition);

  vec3 difspec = LightingFunction(light, bNormal, thePosition, Texcoord);

  if(debug_white_texcoords)
	outColor = vec4(AmbientOcclusion, AmbientOcclusion, AmbientOcclusion, 1.0f); 
  else if(debug_white_color)
	outColor = vec4(vec3(shadow / far_plane), 1.0);
  else
	{
	outColor = vec4(ambientLight + difspec * shadow, 1.0);
  
	vec3 emissive_color;
		if(gamma_correction)
			emissive_color = vec3(pow(texture(texture_emissionl, Texcoord).rgb, vec3(2.2f)));
		else
			emissive_color = vec3(texture(texture_emissionl, Texcoord));

		outColor.rgb += emissive_color;
  
		if(tone_mapping)
			outColor.rgb = vec3(1.0) - exp(-outColor.rgb * hdr_exposure);
	 
		if(gamma_correction)
		outColor.rgb = pow(outColor.rgb, vec3(1.0/2.2f));
	}
};

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal )
{
  //perform perspective divide
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  projCoords = projCoords * 0.5 + 0.5;
  float closestDepth = texture(depth_texture, projCoords.xy).r;
  float currentDepth = projCoords.z;
  float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
  
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
  
  if(LightSpacePos.z > 1.0) //far plane issue!
     shadow = 1.0;
  return clamp((0.5f + (1.0f - shadow)), 0.0f, 1.0f);
} 

float ShadowCalculationCubeMap(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - light.position.xyz;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(depth_cube_map, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return 1.0f - shadow;
} 

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
	float height_scale =0.1f;
	// number of depth layers
    const float numLayers = 10;
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy * height_scale; 
    vec2 deltaTexCoords = P / numLayers;

	// get initial values
	vec2 currentTexCoords     = texCoords;
	float currentDepthMapValue = texture(texture_depth1, currentTexCoords).r;
  
	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(texture_depth1, currentTexCoords).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}
	return currentTexCoords;
}
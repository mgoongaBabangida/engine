#version 430

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
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

out vec4 daColor;

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

layout(binding=1) uniform sampler2DShadow depth_texture; // Shadow
layout(binding=2) uniform sampler2D texture_diffuse1;
layout(binding=3) uniform sampler2D texture_specular1;
layout(binding=4) uniform sampler2D texture_normal1;
layout(binding=5) uniform sampler2D texture_depth1;

uniform vec3 eyePositionWorld;
uniform bool normalMapping = true;

float ShadowCalculation(vec4 fragPosLightSpace);

subroutine(LightingPtr) vec3 calculatePhongPointSpecDif(	Light light, vec3 normal, vec3 thePosition, vec2 TexCoords);
subroutine(LightingPtr) vec3 calculatePhongDirectionalSpecDif(	Light light, vec3 normal, vec3 thePosition, vec2 TexCoords);
subroutine(LightingPtr) vec3 calculatePhongFlashSpecDif(	Light light, vec3 normal, vec3 thePosition, vec2 TexCoords);
//vec3 calculateBlindPhongPointSpecDif(	Light light, vec3 normal, vec3 thePosition, vec2 TexCoords);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{   
	// Paralax mapping
	vec3 tangetnViewDir   = normalize(TBN * eyePositionWorld - TBN * thePosition);
	vec2 fTexCoords = ParallaxMapping(Texcoord,  tangetnViewDir);
	//if(fTexCoords.x > 1.0 || fTexCoords.y > 1.0 || fTexCoords.x < 0.0 || fTexCoords.y < 0.0)
		//discard;
	
	vec3 bNormal;
	if(normalMapping)
	{
		// Obtain normal from normal map in range [0,1]
		bNormal = texture(texture_normal1, fTexCoords).rgb;
		// Transform normal vector to range [-1,1]
		bNormal = normalize(bNormal * 2.0 - 1.0);   
		bNormal = normalize(TBN * bNormal);
	}
	else
		bNormal = theNormal;

  //Ambient
  vec3 ambientLight = light.ambient* vec3(texture(texture_diffuse1, fTexCoords));// * material.ambient  

  float shadow =  ShadowCalculation(LightSpacePos);
  vec3 difspec = LightingFunction(light, bNormal, thePosition, fTexCoords);

  daColor= vec4(ambientLight+(difspec) * shadow, 1.0);
};

subroutine( LightingPtr )
vec3 calculatePhongPointSpecDif(Light light, vec3 normal, vec3 thePosition, vec2 TexCoords)
{
  float shininess = 32.0f;

  //Diffuse
  vec3 lightVector  = normalize(vec3(light.position)-thePosition);
  float Brightness  = clamp(dot(lightVector, normal),0,1);
  vec3 diffuseLight = vec3(light.diffuse * Brightness * vec3(texture(texture_diffuse1, TexCoords)));//Brightness*material.diffuse
  
  //Specular
  vec3 Reflaction = reflect(-lightVector,normal);
  vec3 eyeVector  = normalize(eyePositionWorld-thePosition); 
  float spec      = clamp(dot(Reflaction,eyeVector), 0, 1);

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

subroutine( LightingPtr )
vec3 calculatePhongDirectionalSpecDif(Light light, vec3 normal, vec3 thePosition, vec2 TexCoords)
{
    float shininess =32.0f;
	vec3 lightDir = normalize(vec3(-light.direction));
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
	vec3 eyeVector= normalize(eyePositionWorld-thePosition); 
    float spec = pow(max(dot(eyeVector, reflectDir), 0.0), shininess);
    // combine results
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    return (diffuse + specular);
}

subroutine( LightingPtr )
vec3 calculatePhongFlashSpecDif(Light light, vec3 normal, vec3 viewDir, vec2 TexCoords)
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

subroutine( LightingPtr )
vec3 calculateBlindPhongPointSpecDif(Light light, vec3 normal, vec3 thePosition, vec2 TexCoords) 
{
  float shininess = 32.0f;

  //Diffuse
  vec3 lightVector  = normalize(vec3(light.position)-thePosition);
  float Brightness  = clamp(dot(lightVector, normal),0,1);
  vec3 diffuseLight = vec3(light.diffuse * Brightness * vec3(texture(texture_diffuse1, TexCoords)));//Brightness*material.diffuse
  
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

float ShadowCalculation(vec4 fragPosLightSpace )
{
  float sum=0.0;
  vec2 texelSize = 1.0 / textureSize(depth_texture, 0);
  for(int x = -2; x <= 2; ++x)
    {
       for(int y = -2; y <= 2; ++y)
        {
 		sum += textureProjOffset(depth_texture,fragPosLightSpace,ivec2(0,0));        
        }    
    }    
    sum = sum / 50.0;
    float shadow = 0.3 + sum;

 //if(LightSpacePos.z > 1.0) //far plane issue!
       // shadow = 1.0;
	return shadow;
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
vec2  currentTexCoords     = texCoords;
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
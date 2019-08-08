#version 430

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 Texcoord;
in vec3 thePosition;
in vec3 theNormal;
in mat3 TBN;

layout(binding=2) uniform sampler2D texture_diffuse1;
layout(binding=3) uniform sampler2D texture_specular1;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{    
	// Paralax mapping
	vec3 tangetnViewDir   = normalize(TBN * eyePositionWorld - TBN * thePosition);
	vec2 fTexCoords = ParallaxMapping(Texcoord,  tangetnViewDir);
	
	if(normalMapping)
	{
		// Obtain normal from normal map in range [0,1]
		gNormal = texture(texture_normal1, fTexCoords).rgb;
		// Transform normal vector to range [-1,1]
		gNormal = normalize(bNormal * 2.0 - 1.0);   
		gNormal = normalize(TBN * bNormal);
	}
	else
		gNormal = theNormal; 
    
	gPosition = thePosition;

    gAlbedoSpec.rgb = texture(texture_diffuse1, Texcoords).rgb;
    
    gAlbedoSpec.a = texture(texture_specular1, Texcoords).r;
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
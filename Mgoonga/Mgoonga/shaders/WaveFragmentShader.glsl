#version 430 
 
 struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
};

 struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 daColor;
in vec3 theColor;
in vec3 theNormal;
in vec3 thePosition;
in vec2 Texcoord;
in vec4 LightSpacePos;

uniform Material material;
uniform Light light;
uniform bool normalMap = false;  //+

layout(binding=1) uniform sampler2DShadow depth_texture; 
layout(binding=2) uniform sampler2D texture_diffuse1;
layout(binding=3) uniform sampler2D texture_specular1;
layout(binding=4) uniform sampler2D texture_normal1;

uniform vec3 eyePositionWorld;

float ShadowCalculation(vec4 fragPosLightSpace )
{
  float sum=0;
  float shadow = 0.0;
  //vec2 texelSize = 1.0 / textureSize(depth_texture, 0);
  for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
			sum += textureProjOffset(depth_texture,fragPosLightSpace,ivec2(x,y));        
        }    
    }
    sum /= 50.0;
  shadow=0.3+sum;

 //if(LightSpacePos.z > 1.0) //far plane issue!
       // shadow = 1.0;
	return shadow;
}


void main()
{ 
  
 vec3 finNormal = -theNormal;
	
  //Ambient
  vec3 ambientLight = light.ambient*vec3(texture(texture_diffuse1, Texcoord));// * material.ambient

  //Diffuse
  vec3 lightVector= normalize(light.position-thePosition);
  float Brightness=clamp(dot(lightVector,finNormal),0,1);
  vec3 diffuseLight=vec3(light.diffuse * Brightness * vec3(texture(texture_diffuse1, Texcoord)));//Brightness*material.diffuse
  
  //Specular
  vec3 Reflaction = reflect(-lightVector,finNormal);
  vec3 eyeVector= normalize(eyePositionWorld-thePosition); 
  float spec=clamp(dot(Reflaction,eyeVector),0,1);
  spec=pow(spec, material.shininess);
  vec3 specularLight=vec3(light.specular *spec * vec3(texture(texture_specular1, Texcoord)));//* material.specular
  specularLight=clamp(specularLight,0,1);

  // Attenuation
    float distance    = length(light.position - thePosition);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambientLight  *= attenuation;  
    diffuseLight  *= attenuation;
    specularLight *= attenuation;

  float shadow =  ShadowCalculation(LightSpacePos);

  daColor= vec4(ambientLight+(diffuseLight+specularLight)*shadow,1.0);
  
};

/*
 uniform struct LightInfo { 
     vec4 Position; 
     vec3 Intensity; 
 } Light; 
 
 
 uniform struct MaterialInfo { 
     vec3 Ka; 
     vec3 Kd; 
     vec3 Ks; 
     float Shininess; 
 } Material; 
 
 
 in vec4 Position; 
 in vec3 Normal; 
 in vec2 TexCoord; 
 
 
 uniform float Time; 
 
 
 layout ( location = 0 ) out vec4 FragColor; 
 
 
 vec3 phongModel(vec3 kd) { 
     vec3 n = Normal; 
     if( !gl_FrontFacing ) n = -n; 
     vec3 s = normalize(Light.Position.xyz - Position.xyz); 
     vec3 v = normalize(-Position.xyz); 
     vec3 r = reflect( -s, n ); 
     float sDotN = max( dot(s,n), 0.0 ); 
     vec3 diffuse = Light.Intensity * kd * sDotN; 
     vec3 spec = vec3(0.0); 
     if( sDotN > 0.0 ) 
         spec = Light.Intensity * Material.Ks * 
             pow( max( dot(r,v), 0.0 ), Material.Shininess ); 
 
 
     return Material.Ka * Light.Intensity + diffuse + spec; 
 } 
 
 
 void main() 
{ 
     FragColor = vec4( phongModel(Material.Kd) , 1.0 ); 
} 
*/
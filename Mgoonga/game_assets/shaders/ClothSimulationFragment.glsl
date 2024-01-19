#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform vec4 LightPosition;
uniform vec4 LightIntensity;

uniform vec4 Kd;            // Diffuse reflectivity
uniform vec4 Ka;            // Ambient reflectivity
uniform vec4 Ks;            // Specular reflectivity
uniform float Shininess;    // Specular shininess factor

layout( binding = 0 ) uniform sampler2D Tex;

layout( location = 0 ) out vec4 FragColor;

vec4 ads( )
{
    vec3 s = normalize( vec3(LightPosition) - Position );
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect( -s, Normal );

    return
        LightIntensity * ( Ka +
          Kd * max( dot(s, Normal), 0.0 ) +
          Ks * pow( max( dot(r,v), 0.0 ), Shininess ) );
}

void main() 
{
  vec4 texColor = texture( Tex, TexCoord );
  FragColor = ads() * texColor;
}
#version 430
out vec4 FragColor;

in vec3 normalizedNoisePos;

uniform sampler3D noiseTexture;

uniform vec4 viewDir;
uniform vec4 lightDir;
uniform vec4 cloudColor;
uniform float stepSize = 0.01f;
uniform int noiseSize = 512;
uniform float density = 0.015f;
uniform float perlinWeight = 0.5f; // Weight for Perlin noise effect
uniform float absorption = 0.05f;
uniform float g = 0.5f; // Henyey-Greenstein parameter
uniform float scatteringCoefficient = 2.0f; // Powdered sugar

uniform float time; // Uniform variable for time
uniform float perlinMotionScale = 0.1f; // Scale for Perlin noise motion
uniform float worleyMotionScale = 0.1f; // Scale for Worley noise motion

vec4 sampleVolume(vec3 texCoord, float distance)
{
	// Apply Brownian motion
    vec3 perlinTexCoord = texCoord + vec3(sin(time * perlinMotionScale), cos(time * perlinMotionScale), sin(time * perlinMotionScale * 0.5));
    vec3 worleyTexCoord = texCoord + vec3(cos(time * worleyMotionScale), sin(time * worleyMotionScale), cos(time * worleyMotionScale * 0.5));
	
	vec4 noise = texture(noiseTexture, worleyTexCoord);
	float worleyVal  = noise.r * 0.625 + noise.g * 0.25 + noise.b * 0.125;
	
	float perlinVal = texture(noiseTexture, perlinTexCoord).a;
	
	// Modulate Worley noise with Perlin noise
    float combinedVal = worleyVal * (1.0 + perlinWeight * (perlinVal - 0.5));
	
	 // Adjust the density based on distance
	combinedVal *= density;
	
    return vec4(vec3(cloudColor), combinedVal);
}

float henryGreenstein(float g, float cosTheta)
{
    float g2 = g * g;
    float denom = 1.0 + g2 - 2.0 * g * cosTheta;
    float hg = (1.0 - g2) / pow(denom, 1.5);
    return max(hg, 0.01); // Ensure a minimum scattering value
}

void main()
{
    vec3 rayOrigin = normalizedNoisePos;
    vec3 rayDir = normalize(vec3(viewDir));

    vec4 accumulatedColor = vec4(0.0);
    vec4 sampleRes = vec4(0.0);
    vec3 texCoord = rayOrigin;
	float totalDistance = 0.0f;
	
    for (int i = 0; i < noiseSize; ++i)
    {
        if (texCoord.x < -0.01 || texCoord.x > 1.01 || texCoord.y < -0.01 || texCoord.y > 1.01 || texCoord.z < -0.01 || texCoord.z > 1.01)
            break;
			
		float distance = length(texCoord - rayOrigin);
        sampleRes = sampleVolume(texCoord, distance);
		
		// Calculate scattering
		float cosTheta = dot(normalize(rayDir), normalize(vec3(lightDir)));
		// Calculate the Henyey-Greenstein phase function
        float hgPhase = henryGreenstein(g, cosTheta);
		
		// Apply Beer's law for light attenuation
        float absorptionCoef = exp(-totalDistance * absorption);
		// Powderd sugar
		float powderCoef = 1 - exp(-totalDistance * scatteringCoefficient);
		// Combine absorption and scattering
        float combinedEffect = absorptionCoef + powderCoef - (absorptionCoef * powderCoef);
		
		vec3 attenuatedLight = sampleRes.rgb * combinedEffect;
				
		// Apply the phase function to the sample
        vec3 scatteredLight = attenuatedLight * hgPhase;
		
		// Calculate isotropic scattering
        //float scattering = max(dot(rayDir, vec3(lightDir))* 0.5 + 0.5, 0.1);
        //vec3 scatteredLight = sampleRes.rgb * scattering;
		
        accumulatedColor.rgb += (1.0 - accumulatedColor.a) * scatteredLight * sampleRes.a;
        accumulatedColor.a += (1.0 - accumulatedColor.a) * sampleRes.a;

        if (accumulatedColor.a >= 1.0)
            break;

        texCoord += rayDir * stepSize;
		totalDistance += stepSize;
    }
		
    FragColor = accumulatedColor;
}

#version 430
out vec4 FragColor;

in vec3 normalizedNoisePos;
in vec4 worldPos;

uniform sampler3D noiseTexture;

uniform vec4 cameraPos;
uniform vec4  lightDir;
uniform vec4  cloudColor;
uniform float stepSize = 0.01f;
uniform int   noiseSize = 512;
uniform float density = 0.015f;
uniform float perlinWeight = 0.5f; // Weight for Perlin noise effect
uniform float absorption = 0.05f;
uniform float g = 0.5f; // Henyey-Greenstein parameter
uniform float scatteringCoefficient = 2.0f; // Powdered sugar
uniform bool  apply_powder = true;
uniform float alphaThreshold = 0.05f; // Alpha threshold for discarding fragments
uniform bool  silver_lining = true;
uniform float silver_lining_density = 0.7f;
uniform int   silver_lining_strength = 7;

uniform float time; // Uniform variable for time
uniform float perlinMotionScale = 0.1f; // Scale for Perlin noise motion
uniform float worleyMotionScale = 0.1f; // Scale for Worley noise motion
uniform vec4  noiseScale = vec4(1.0f); // Scale for the noise textures in each dimension;

vec4 sampleVolume(vec3 texCoord)
{
	// Scale the texture coordinates in each dimension
    texCoord *= vec3(noiseScale);
	
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

float computeOpticalThickness(vec3 start, vec3 direction, int steps, float stepSize)
{
    float thickness = 0.0;

    for (int i = 0; i < steps; ++i)
    {
        vec3 texCoord = start + direction * (i * stepSize);
        if (texCoord.x < -0.01 || texCoord.x > 1.01 || texCoord.y < -0.01 || texCoord.y > 1.01 || texCoord.z < -0.01 || texCoord.z > 1.01)
            break;

        float densitySample = sampleVolume(texCoord).a;
		thickness += (1.0 - thickness) * densitySample * (silver_lining_density * 10);
		
		if (thickness >= 1.0)
            break;
    }
    return thickness;
}

void main()
{
    vec3 rayOrigin = normalizedNoisePos;
    vec3 rayDir = normalize(vec3(worldPos-cameraPos));

    vec4 accumulatedColor = vec4(0.0);
    vec4 sampleRes = vec4(0.0);
    vec3 texCoord = rayOrigin;
	float totalDistance = 0.0f;
	float opticalThickness = 0.0f;
	
    for (int i = 0; i < noiseSize; ++i)
    {
        if (texCoord.x < -0.01 || texCoord.x > 1.01 || texCoord.y < -0.01 || texCoord.y > 1.01 || texCoord.z < -0.01 || texCoord.z > 1.01)
            break;
			
        sampleRes = sampleVolume(texCoord);
		
		// Calculate scattering
		float cosTheta = dot(normalize(rayDir), normalize(vec3(lightDir)));
		// Calculate the Henyey-Greenstein phase function
        float hgPhase = henryGreenstein(g, cosTheta);
		
		// Apply Beer's law for light attenuation
        float absorptionCoef = exp(-totalDistance * absorption);
		// Powderd sugar
		float powderCoef = apply_powder ? 1 - exp(-totalDistance * scatteringCoefficient) : 1.0f;
		// Combine absorption and scattering
        float combinedEffect = absorptionCoef * powderCoef;
		
		vec3 attenuatedLight = sampleRes.rgb * combinedEffect;
				
		// Apply the phase function to the sample
        vec3 scatteredLight = attenuatedLight * hgPhase;
		
		// Calculate optical thickness from rayOrigin towards the light direction
		//if(silver_lining)
		//	opticalThickness += computeOpticalThickness(texCoord, -normalize(vec3(lightDir)), noiseSize, stepSize*10);
	
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
	
	opticalThickness = computeOpticalThickness(rayOrigin, -normalize(vec3(lightDir)), noiseSize, stepSize);
	float illuminationFactor = 1 - opticalThickness;
	illuminationFactor *= accumulatedColor.a;
		
	if(silver_lining)
	{
		accumulatedColor.rgb *= pow(1.f + illuminationFactor, silver_lining_strength );
	}
			
	// Discard fragments with low alpha values
    if (accumulatedColor.a < alphaThreshold)
        discard;
		
    FragColor = accumulatedColor;
}

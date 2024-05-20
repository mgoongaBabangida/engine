#version 430
out vec4 FragColor;

in vec3 normalizedNoisePos;

uniform sampler3D noiseTexture;

uniform vec4 viewDir;
uniform float stepSize = 0.01f;
uniform int noiseSize = 512;

uniform float coef = 0.01f;

vec4 sampleVolume(vec3 texCoord)
{
	float val = texture(noiseTexture, texCoord).r;
	//return vec4(1., 1., 1., 1.);
    return vec4(1., 1., 1., val * coef);
}

void main()
{
    vec3 rayOrigin = normalizedNoisePos;
    vec3 rayDir = normalize(vec3(viewDir));

    vec4 accumulatedColor = vec4(0.0);
    vec4 sampleRes = vec4(0.0);
    vec3 texCoord = rayOrigin;

    for (int i = 0; i < noiseSize; ++i)
    {
        if (texCoord.x < 0.0 || texCoord.x > 1.0 || texCoord.y < 0.0 || texCoord.y > 1.0 || texCoord.z < 0.0 || texCoord.z > 1.0)
            break;

        sampleRes = sampleVolume(texCoord);
        accumulatedColor.rgb += (1.0 - accumulatedColor.a) * sampleRes.rgb * sampleRes.a;
        accumulatedColor.a += (1.0 - accumulatedColor.a) * sampleRes.a;

        if (accumulatedColor.a >= 1.0)
            break;

        texCoord += rayDir * stepSize;
    }

    FragColor = accumulatedColor;
}

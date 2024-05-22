#include "stdafx.h"
#include "Utils.h"

namespace dbb
{
	//-------------------------------------------------------
	float round(float _x, int _num_decimal_precision_digits)
	{
		float power_of_10 = static_cast<float>(std::pow(10, _num_decimal_precision_digits));
		return std::round(_x * power_of_10) / power_of_10;
	}

  //-------------------------------------------------------
	std::vector<float> generatePerlinNoise3D(int width, int height, int depth)
  {
    std::vector<float> noiseData(width * height * depth, 0.0f);
    float max = 0.;
    float min = 1.;

    float _persistance = 0.5;
    float _lacunarity = 2.0;
    float _scale = 0.1f;

    float mChannels = 4; //octaves

    float xFactor = 1.0f / (width - 1);
    float yFactor = 1.0f / (height - 1);
    float zFactor = 1.0f / (depth - 1);

    for (uint32_t row = 0; row < height; row++)
    {
      for (uint32_t col = 0; col < width; col++)
      {
        for (uint32_t dpt = 0; dpt < depth; dpt++)
        {
          float x = xFactor * col;
          float y = yFactor * row;
          float z = zFactor * dpt;

          float amplitude = 1.0f;
          float frequency = 1.0f;

          float sum = 0.0f;
          for (int oct = 0; oct < mChannels; ++oct)
          {
            glm::vec3 p(x / _scale * frequency, y / _scale * frequency, z / _scale * frequency);
            float val = (glm::perlin(p, glm::vec3(width)) + 1.0f) / 2.0f * amplitude;

            sum += val;

            amplitude *= _persistance;
            frequency *= _lacunarity;
          }
          noiseData[row + col * width + dpt * width * height] = sum;
          if (sum > max)
            max = sum;
          if (sum < min)
            min = sum;
        }
      }
    }

    for (uint32_t i = 0; i < noiseData.size(); i++)
      noiseData[i] = noiseData[i] / max;
    return noiseData;
  }
}

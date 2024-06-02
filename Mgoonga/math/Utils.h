#pragma once

#include "math.h"

#include <cmath>
#include <vector>

#include <glm/glm/gtc/noise.hpp>

namespace dbb
{
	DLL_MATH float round(float _x, int _num_decimal_precision_digits);

	template<typename T>
	std::vector<T>& Unite(std::vector<T>& _modified, const std::vector<T>& _second)
	{
		_modified.reserve(_modified.size() + _second.size());
		_modified.insert(_modified.end(), _second.begin(), _second.end());
		return _modified;
	}

  // Function to generate 3D Perlin noise with 4 octaves and periodicity
	DLL_MATH std::vector<float> generatePerlinNoise3D(int width, int height, int depth);

  DLL_MATH bool WriteToFile(const std::vector<float>& _data, const std::string& _filename);
  DLL_MATH bool ReadFromFile(std::vector<float>& _data, const std::string& _filename);
}

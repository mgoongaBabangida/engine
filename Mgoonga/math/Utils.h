#pragma once

#include "math.h"

#include <cmath>
#include <vector>

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
}

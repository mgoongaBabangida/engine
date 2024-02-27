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
}

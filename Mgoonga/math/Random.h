#pragma once
#include <chrono>
#include <random>

namespace math {

	//-----------------------------------------------------------------------------
	class Random
	{
	public:
		static float RandomFloat(float from, float to, int seed = 0)
		{
			std::uniform_real_distribution<float> randomFloats(from, to);
			if (seed == 0)
			{
				std::default_random_engine generator{ static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) };
				return randomFloats(generator);
			}
			else
			{
				std::default_random_engine generator{ static_cast<long unsigned int>(seed) };
				return randomFloats(generator);
			}
		}

		static int32_t RandomInt(int32_t from, int32_t to, int seed = 0)
		{
			std::uniform_int_distribution<int32_t> randomInts(from, to);
			if (seed == 0)
			{
				std::default_random_engine generator{ static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) };
				return randomInts(generator);
			}
			else
			{
				std::default_random_engine generator{ static_cast<long unsigned int>(seed) };
				return randomInts(generator);
			}
		}
	};

}
#pragma once

class Random
{
public:
	static float RandomFloat(float from, float to)
	{
		std::uniform_real_distribution<float> randomFloats(from, to);
		std::default_random_engine generator{ static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) };
		return randomFloats(generator);
	}

};
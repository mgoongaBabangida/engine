#include "stdafx.h"
#include "Timer.h"
#include <thread>
#include <future>

namespace math {

	Timer::Timer(std::function<bool()> c) :callable(c)
	{

	}

	bool Timer::start(unsigned int period)
	{
		active = true;
		clock.start();
		std::function<bool()> func = [this, period]()->bool {
			while (active)
			{
				if (clock.timeEllapsedLastFrameMsc() >= period)
				{
					this->callable();
					this->clock.newFrame();
				}
			}
			return true;
		};

		fut = std::async(func);
		return true;
	}

	bool Timer::stop()
	{
		if (active)
		{
			active = false;
			fut.get();
			return true;
		}
		return false;
	}

	Timer::~Timer()
	{
		this->stop();
	}
}
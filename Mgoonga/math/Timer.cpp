#include "stdafx.h"
#include "Timer.h"
#include <thread>
#include <future>

namespace math {

	Timer::Timer(std::function<bool()> c) :callable(c), active(false)
	{

	}

	bool Timer::start(unsigned int period)
	{
		bool fls = false;
		if (!active.compare_exchange_weak(fls, true))
		{
			return false;
		}
		else
		{
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
	}

	bool Timer::stop()
	{
		bool is_true = true;
		if (active.compare_exchange_strong(is_true, false))
		{
			if(fut.valid())
				fut.get();
			else
				return false;
			return true;
		}
		return false;
	}

	Timer::~Timer()
	{
		this->stop();
	}
}
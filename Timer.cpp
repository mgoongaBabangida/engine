#include "Timer.h"
#include <thread>
#include <future>

dbb::Timer::Timer(std::function<bool()> c):callable(c)
{

}

bool dbb::Timer::start(unsigned int period)
{
	active = true;
	clock.start();
	std::function<bool()> func = [this,period]()->bool {
		while(active)
		{
			if (clock.timeEllapsedLastFrameMsc() >= period)
			{
				this->callable();
				this->clock.newFrame();
			}

		}		
		return true;
	};

	std::packaged_task<bool()> tsk(func);
	fut = tsk.get_future();
	thrd = std::thread(std::move(tsk));

	return true;
}

bool dbb::Timer::stop()
{
	if(active)
	{
		active = false;
		fut.get();
		if (thrd.joinable())
			thrd.join();
		return true;
	}
	return false;
}

dbb::Timer::~Timer()
{
	this->stop();
}

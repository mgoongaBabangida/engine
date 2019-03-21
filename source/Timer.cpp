#include "stdafx.h"
#include "Timer.h"

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
	if(thrd.joinable())
		thrd.join();
	active = false;
	fut.get();
	return true;
}

dbb::Timer::~Timer()
{
	this->stop();
}

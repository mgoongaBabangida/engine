#ifndef TIMER_H
#define TIMER_H

#include "Clock.h"
#include <functional>
#include <thread>
#include <future>

namespace math {

	class DLL_MATH Timer
	{
	 public:
		 Timer(std::function<bool()> callable);

		 Timer(const Timer&) = delete;
		 Timer& operator=(const Timer&) = delete;

		 virtual bool start(unsigned int period);
		 virtual bool stop();
		 virtual ~Timer();
	 protected:
		 eClock clock;
		 std::atomic<bool> active;
		 std::function<bool()> callable;
		 std::future<bool> fut;
	};

};
#endif

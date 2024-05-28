#ifndef CLOCK_H
#define CLOCK_H

#include <chrono>

#include "math.h"

namespace math {

	//--------------------------------------------------------------------
	class DLL_MATH eClock
	{
	public:
		void			start();
		void			reset();
		void			pause();
		void			goOn();
		void			restart();

		int64_t		newFrame(); //returns time since last frame
		int64_t		timeElapsedMsc(); // since start
		int64_t		timeElapsedLastFrameMsc();

		bool			isActive();
		bool			isPaused();

	protected:
		std::chrono::system_clock::time_point  m_start;
		std::chrono::system_clock::time_point  last_frame;
		std::chrono::system_clock::time_point  pause_time;
		bool								   paused = false;
	};
}

#endif

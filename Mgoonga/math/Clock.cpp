#include "stdafx.h"
#include "Clock.h"

namespace math {

	//-------------------------------------------------------------
	void eClock::reset()
	{
		m_start = last_frame = pause_time = std::chrono::system_clock::time_point();
		paused = false;
	}

	//-------------------------------------------------------------
	void eClock::start()
	{
		if (m_start == std::chrono::system_clock::time_point())
			m_start = last_frame = std::chrono::system_clock::now();
	}

	//-------------------------------------------------------------
	int64_t eClock::timeElapsedMsc()
	{
		if (!paused)
		{
			auto now = std::chrono::system_clock::now();
			std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start);
			return duration.count();
		}
		else
		{
			std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(pause_time - m_start);
			return duration.count();
		}
	}

	//-------------------------------------------------------------
	int64_t eClock::newFrame()
	{
		if (!paused)
		{
			auto now = std::chrono::system_clock::now();
			std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame);
			last_frame = now;
			return duration.count();
		}
		else
		{
			std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(pause_time - last_frame);
			last_frame = pause_time;
			return duration.count();
		}
	}

	//-------------------------------------------------------------
	int64_t eClock::timeElapsedLastFrameMsc()
	{
		if (!paused)
		{
			auto now = std::chrono::system_clock::now();
			std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame);
			return duration.count();
		}
		else
		{
			std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(pause_time - last_frame);
			return duration.count();
		}
	}

	//-------------------------------------------------------------
	void eClock::restart()
	{
		m_start = last_frame = std::chrono::system_clock::now();
		paused = false;
	}

	//-------------------------------------------------------------
	bool eClock::isActive()
	{
		if (m_start == std::chrono::system_clock::time_point()) // && !paused
			return false;
		else
			return true;
	}

	//-------------------------------------------------------------
	void eClock::pause()
	{
		pause_time = std::chrono::system_clock::now();
		paused = true;
	}

	//-------------------------------------------------------------
	void eClock::goOn()
	{
		if (paused) // if pasued time > m_start
		{
			auto now = std::chrono::system_clock::now();
			std::chrono::milliseconds paused_period = std::chrono::duration_cast<std::chrono::milliseconds>(now - pause_time);
			m_start += paused_period;
			last_frame += paused_period;
			paused = false;
		}
	}
	bool eClock::isPaused()
	{
		return paused;
	}
}


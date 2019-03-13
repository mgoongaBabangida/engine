#include "stdafx.h"
#include "Clock.h"

void Clock::start()
{
	if(m_start == std::chrono::system_clock::time_point() )
	m_start = last_frame = std::chrono::system_clock::now();
}

int Clock::timeEllapsedMsc()
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

int Clock::newFrame()
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

int Clock::timeEllapsedLastFrameMsc()
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

void Clock::restart()
{
	m_start = last_frame = std::chrono::system_clock::now();
	paused = false;
}

bool Clock::isActive()
{
	if (m_start == std::chrono::system_clock::time_point()) // && !paused
		return false;
	else
		return true;
}

void Clock::pause()
{
	pause_time = std::chrono::system_clock::now();
	paused = true;
}

void Clock::goOn()
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


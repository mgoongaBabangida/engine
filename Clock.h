#ifndef CLOCK_H
#define CLOCK_H
#include <chrono>

class Clock
{
protected:
	std::chrono::system_clock::time_point  m_start;
	std::chrono::system_clock::time_point  last_frame;
	std::chrono::system_clock::time_point  pause_time;
	bool paused = false;

public:
	void		start();
	int64_t		timeEllapsedMsc();
	int			newFrame();
	int			timeEllapsedLastFrameMsc();
	void		restart();
	bool		isActive();
	void		pause();
	void		goOn();
	bool		isPaused() { return paused; }
};

#endif

#ifndef SCELETAL_ANIMATION_H 
#define SCELETAL_ANIMATION_H

#include "Transform.h"
#include "Clock.h"

struct Frame
{
	int									 timeStamp; //can be const?
	std::map<std::string, Transform>	 pose; //45
	
	Frame(int timeStamp, std::map<std::string, Transform> pose) :timeStamp(timeStamp), pose(pose) {};
	Frame(){} //improve
	
	void addTimeStemp(int stamp)						{ timeStamp = stamp; }
	void addTrnasform(std::string name, Transform trs)  { pose.insert(std::pair<std::string, Transform>(name, trs)); }
	bool exists(const std::string& name) const			{ return pose.find(name) != pose.end(); }
};

class SceletalAnimation
{
protected:
	Clock				clock;
	std::vector<Frame>  frames;
	int					duration; // msc
	std::string			name;
public:
	SceletalAnimation(int dur, const std::vector<Frame> &  frames ,const std::string _name) 
		:duration(dur), frames(frames),name(_name)	{ Start(); } // to del }
	
	bool operator==(const SceletalAnimation& other) { return name == other.name && duration == other.duration; }
	
	Frame getCurrentFrame(); 
	void Start()						{ clock.start(); }
	void Stop()							{ clock.pause(); }
	void Continue()						{ clock.goOn(); }
	bool IsPaused()						{return clock.isPaused(); }
	const std::string& Name()const		{ return name; }
	void SetName(const std::string& n)	{ name = n; }

	void Debug();
};

#endif // ! 


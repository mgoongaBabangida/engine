#ifndef SCELETAL_ANIMATION_H 
#define SCELETAL_ANIMATION_H

#include "stdafx.h"
#include <base/interfaces.h>

#include "Transform.h"
#include "Clock.h"

#include <map>


//-------------------------------------------------------------------
struct Frame
{
	int									 timeStamp; //can be const?
	std::map<std::string, Transform>	 pose; //45
	
	Frame(int timeStamp, std::map<std::string, Transform> pose) 
    : timeStamp(timeStamp), pose(pose) 
  {}
	Frame(){} //improve
	
	void addTimeStemp(int stamp)						            { timeStamp = stamp; }
	void addTrnasform(std::string name, Transform trs)  { pose.insert(std::pair<std::string, Transform>(name, trs)); }
	bool exists(const std::string& name) const			    { return pose.find(name) != pose.end(); }
};

//---------------------------------------------------------------------
class DLL_MATH SceletalAnimation : public IAnimation
{
public:
	SceletalAnimation(int dur, const std::vector<Frame> &  frames ,const std::string _name) 
		:duration(dur), frames(frames),name(_name)	{ Start(); } // to del }
	
	bool operator==(const SceletalAnimation& other) { return name == other.name && duration == other.duration; }
	
	const Frame& getCurrentFrame();

	virtual void Start() override;
	virtual void Stop() override;
	virtual void Continue() override;
	virtual bool IsPaused() override;
	virtual const std::string& Name() const		override;
	void SetName(const std::string& n);

	void Debug();

protected:
  math::eClock		clock;
  std::vector<Frame>  frames;
  int					duration; // msc
  std::string			name;
};

#endif 


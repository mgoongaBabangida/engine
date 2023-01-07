#include "stdafx.h"
#include "SceletalAnimation.h"
#include <iostream>

const Frame& SceletalAnimation::getCurrentFrame()
{
	if(clock.timeEllapsedMsc() > duration) //need to improve
		clock.restart();

	float time = clock.timeEllapsedMsc();
	//std::cout << "time " << time << std::endl;
	int i = 0;

	for (; i < frames.size() - 1; ++i)
	{
		if(frames[i].timeStamp > time) //should be sorted
			break;
	}
	if(i == 0) 
		{ return frames[0]; }
	return frames[i];

	//float progression = (time - frames[i - 1].timeStamp) / (frames[i].timeStamp - frames[i - 1].timeStamp);	
	/*std::cout << "Debug progression " << progression <<" "<< i << std::endl;
	std::cout << "i " << i << std::endl;*/

	//std::map<std::string, Transform> newFrame;
	////names !?
	//for (auto & entry : frames[i - 1].pose)
	//{
	//	auto dest = *(frames[i].pose.Find(entry.first));
	//	Transform trans = Transform::interpolate(entry.second, dest.second, progression);
	//	newFrame.insert(std::pair<std::string, Transform>(entry.first, trans));
	//}

	//return Frame(time, newFrame);
}

void SceletalAnimation::Start()
{
	clock.start();
}

void SceletalAnimation::Stop()
{
	clock.pause();
}

void SceletalAnimation::Continue()
{
	clock.goOn();
}

bool SceletalAnimation::IsPaused()
{
	return clock.isPaused();
}

const std::string& SceletalAnimation::Name() const
{
	return name;
}

void SceletalAnimation::SetName(const std::string& n)
{
	name = n;
}

void SceletalAnimation::Debug()
{
	std::cout << "--------Time-----" << " " << std::endl;
	for (auto &fr : frames)
	{
		for (auto& ps : fr.pose)
		{
			std::cout << ps.first << " " << std::endl;
			std::cout << ps.second.getModelMatrix()[0][0] <<" " << ps.second.getModelMatrix()[0][1]<< " " << ps.second.getModelMatrix()[0][2] << " " << ps.second.getModelMatrix()[0][3] <<std::endl;
			std::cout << ps.second.getModelMatrix()[1][0] << " " << ps.second.getModelMatrix()[1][1] << " " << ps.second.getModelMatrix()[1][2] << " " << ps.second.getModelMatrix()[1][3] << std::endl;
			std::cout << ps.second.getModelMatrix()[2][0] << " " << ps.second.getModelMatrix()[2][1] << " " << ps.second.getModelMatrix()[2][2] << " " << ps.second.getModelMatrix()[2][3] << std::endl;
		}
	}
}


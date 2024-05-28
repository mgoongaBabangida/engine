#include "stdafx.h"
#include "SceletalAnimation.h"

const Frame& SceletalAnimation::getCurrentFrame()
{
	if (freeze_frame != -1)
	{
		return freeze_frame < frames.size() ? frames[freeze_frame] : frames[frames.size()-1];
	}

	if (clock.timeElapsedMsc() > duration)
	{
		if (play_once)
			clock.pause();
		else
			clock.restart();
	}

	int64_t time = clock.timeElapsedMsc();
	size_t i = 0;

	for (; i < frames.size() - 1; ++i)
	{
		if(frames[i].timeStamp > time) //should be sorted
			break;
	}
	cur_frame_index = i;
	return frames[i];

	//interpolation
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

const Frame& SceletalAnimation::GetFrameByNumber(size_t _num) const
{
	if (_num < frames.size())
		return frames[_num];
	else
		return frames[frames.size() - 1];
}

size_t SceletalAnimation::GetCurFrameIndex() const
{
	return freeze_frame != -1 ? freeze_frame : cur_frame_index;
}

void SceletalAnimation::Start()
{
	play_once = false;
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

void SceletalAnimation::PlayOnce()
{
	play_once = true;
	clock.restart();
}

void SceletalAnimation::FreezeFrame(size_t _frame)
{
	freeze_frame = _frame;
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


#include "stdafx.h"
#include "SoundManager.h"
#include "Sound.h"
#include <algorithm>

eSoundManager::eSoundManager(const std::string& _path)
: assetsFolderPath(_path)
{
	context.reset(new SoundContext());
	context->init();
	context->LoadWavFile(assetsFolderPath + "Cannon+5.wav");

	sounds.push_back(std::unique_ptr<RemSnd>(new RemSnd(context->buffers.find(assetsFolderPath + "Cannon+5.wav")->second, true, "shot_sound")));
}

SoundContext* eSoundManager::GetContext()
{
	return context.get();
}

RemSnd* eSoundManager::GetSound(const std::string& _name)
{
	//$todo sounds should be copied 
	return (std::find_if(sounds.begin(), sounds.end(), [&_name](const std::unique_ptr<RemSnd>& snd)->bool 
														{ 
															return snd->Name() == _name;  
														}))->get();
}

eSoundManager::~eSoundManager()
{
	//$todo check if need destructor to clean up
}

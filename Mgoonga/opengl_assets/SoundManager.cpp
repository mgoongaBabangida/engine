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
	context->LoadWavFile(assetsFolderPath + "page-flip-01a.wav");
	context->LoadWavFile(assetsFolderPath + "fire-1.wav");
	context->LoadWavFile(assetsFolderPath + "seagull_sound.wav");

	sounds.push_back(std::unique_ptr<RemSnd>(new RemSnd(context->GetBuffers().find(assetsFolderPath + "Cannon+5.wav")->second, true, "shot_sound")));
	sounds.push_back(std::unique_ptr<RemSnd>(new RemSnd(context->GetBuffers().find(assetsFolderPath + "page-flip-01a.wav")->second, false, "page_sound")));
	sounds.push_back(std::unique_ptr<RemSnd>(new RemSnd(context->GetBuffers().find(assetsFolderPath + "fire-1.wav")->second, false, "fire_sound")));
	sounds.push_back(std::unique_ptr<RemSnd>(new RemSnd(context->GetBuffers().find(assetsFolderPath + "seagull_sound.wav")->second, true, "seagull_sound")));
}

SoundContext* eSoundManager::GetContext()
{
	return context.get();
}

RemSnd* eSoundManager::GetSound(const std::string& _name)
{
	//@todo sounds should be copied 
	return (std::find_if(sounds.begin(), sounds.end(), [&_name](const std::unique_ptr<RemSnd>& snd)->bool 
														{ 
															return snd->Name() == _name;
														}))->get();
}

eSoundManager::~eSoundManager()
{
  context->exit();
}

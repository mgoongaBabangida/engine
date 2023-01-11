#pragma once

#include "stdafx.h"
#include <opengl_assets\opengl_assets.h>

class SoundContext;
class RemSnd;

//-----------------------------------------------------------
class DLL_OPENGL_ASSETS eSoundManager
{
public:
	explicit eSoundManager(const std::string&);
	SoundContext*					GetContext();
	RemSnd*							  GetSound(const std::string&); //should clone and give control
	~eSoundManager();

protected:
	std::unique_ptr<SoundContext>			    context;
	std::vector<std::unique_ptr<RemSnd>>	sounds; //should clone and give control
	std::string								            assetsFolderPath;
};

#pragma once

#include <memory>
#include <string>
#include <vector>

class SoundContext;
class RemSnd;

class eSoundManager
{
public:
	explicit eSoundManager(const std::string&);
	SoundContext*					GetContext();
	RemSnd*							GetSound(const std::string&);
	~eSoundManager();

protected:
	std::unique_ptr<SoundContext>			context;
	std::vector<std::unique_ptr<RemSnd>>	sounds;
	std::string								assetsFolderPath;
};

#pragma once

#include <AL\include\al.h>
#include <AL\include\alc.h>

#include <base/interfaces.h>

#include <map>
#include <string>

//#pragma comment(lib, "OpenAL32.lib")

class SoundContext //singleton
{
public:
	bool init();
	bool exit();
	std::map<std::string, ALuint> buffers;

	bool		LoadWavFile(const std::string &Filename);

private:
	ALCdevice*	device;
	ALCcontext* context;
	bool		endWithError(std::string str);
};

//---------------------------------------------------------
class RemSnd : public ISound
{
public:
	ALfloat mVel[3];
	ALfloat mPos[3];
	bool	mLooped;
	bool	playing;

	// Construction/destruction
	RemSnd(ALuint buffer, bool looped, const std::string& _name = "empty");
	RemSnd(const RemSnd& cSnd);
	virtual ~RemSnd();

	// Functions
	bool				Open(const std::string &Filename, bool Looped, bool Streamed);
	bool				IsStreamed();
	void				Play();
	void				Close();
	void				Update();
	void				Move(float X, float Y, float Z);
	void				Stop();
	bool				isPlaying() { return playing; }
	const std::string&	Name() const { return mName; }

	void		connectToBuffer(ALuint buffer);
	static bool init();
	static bool exit();
	static void loadListner(float x, float y, float z);

private:
	ALuint      mSourceID;
	bool		mStreamed;
	std::string mName;
};




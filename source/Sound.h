#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <map>
#include <string>

//#pragma comment(lib, "OpenAL32.lib")

class SoundContext //singleton
{
	ALCdevice* device;
	ALCcontext* context;
	bool endWithError(std::string str);
public:
	bool init();
	bool exit();
	std::map<std::string, ALuint> buffers;
	bool LoadWavFile(const std::string &Filename);
};

class remSnd
{
public:
	ALfloat mVel[3];
	ALfloat mPos[3];
	bool  mLooped;
	bool playing;

	// Functions
	bool Open(const std::string &Filename, bool Looped, bool Streamed);
	bool IsStreamed();
	void Play();
	void Close();
	void Update();
	void Move(float X, float Y, float Z);
	void Stop();
	bool isPlaying() { return playing; }

	// Construction/destruction
	remSnd(ALuint buffer, bool looped);
	remSnd(const remSnd& cSnd);
	void connectToBuffer(ALuint buffer);
	virtual ~remSnd();
	static bool init();
	static bool exit();
	static void loadListner(float x, float y, float z);
private:
	// Идентификатор источника
	ALuint      mSourceID;
	// Потоковый ли наш звук?
	bool      mStreamed;
};




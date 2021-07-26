#include "stdafx.h"
#include "Sound.h"
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <Windows.h>

bool SoundContext::endWithError(std::string str)
{
	std::cout << str << std::endl;
	return false;
}

//-----------------------------------------------------------------------------
RemSnd::RemSnd(const RemSnd& cSnd) 
: mLooped(cSnd.mLooped), mSourceID(cSnd.mSourceID), mStreamed(cSnd.mStreamed) 
{ 
	playing = false; 
}

bool RemSnd::Open(const std::string & Filename, bool Looped, bool Streamed)
{
	return false;
}

bool RemSnd::IsStreamed()
{
	return false;
}

void RemSnd::Play()
{
	alSourcePlay(mSourceID);
	playing = true;
}

void RemSnd::Close()
{
	alSourceStop(mSourceID);
	if (alIsSource(mSourceID)) alDeleteSources(1, &mSourceID);
}

void RemSnd::Update()
{
}

void RemSnd::Move(float X, float Y, float Z)
{
	ALfloat Pos[3] = { X, Y, Z };
	alSourcefv(mSourceID, AL_POSITION, Pos);
}

void RemSnd::Stop()
{
	alSourceStop(mSourceID);
	playing = false;
}

RemSnd::RemSnd(ALuint buffer, bool looped, const std::string& _name)
{
	ALfloat SourcePos[] = { 0.0,0.0,0.0 };
	ALfloat SourceVel[] = { 0.0,0.0,0.0 };
	mLooped = looped;
	alGenSources(1, &mSourceID);
	alSourcei(mSourceID, AL_BUFFER, buffer);
	alSourcef(mSourceID, AL_PITCH, 1.0f);
	alSourcef(mSourceID, AL_GAIN, 1.0f);
	alSourcefv(mSourceID, AL_POSITION, SourcePos);
	alSourcefv(mSourceID, AL_VELOCITY, SourceVel);
	//alSourcei(mSourceID, AL_BUFFER, AL_FALSE);
	alSourcei(mSourceID, AL_LOOPING, mLooped);
	mName = _name;
}

void RemSnd::connectToBuffer(ALuint buffer)
{
	alSourcei(mSourceID, AL_BUFFER, buffer);
}

RemSnd::~RemSnd()
{
	alSourceStop(mSourceID);
	 if(alIsSource(mSourceID))
     alDeleteSources(1, &mSourceID);
}

bool SoundContext::init()
{
	device = alcOpenDevice(NULL);
	if (!device) 
		return endWithError("no sound device");
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	if (!context) 
		return endWithError("no sound context");
	return true;
}

bool SoundContext::exit()
{
	/*for( auto& buf:buffers)
		alDeleteBuffers(1, &buf.second);*/

	alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);
	return true;
}

void RemSnd::loadListner(float x, float y, float z)
{
	ALfloat ListnerPos[] = {x, y, z };
	ALfloat ListnerVel[] = { 0.0 ,1.0, 0.0 };
	ALfloat ListnerOri[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };

	//associate data with buf
	alListenerfv(AL_POSITION, ListnerPos);
	alListenerfv(AL_VELOCITY, ListnerVel);
	alListenerfv(AL_ORIENTATION, ListnerOri);
}

bool SoundContext::LoadWavFile(const std::string & Filename)
{
	FILE* fp = NULL;
	fopen_s(&fp, Filename.c_str(), "rb");
	////create variables to store file info
	char type[4];
	DWORD size, chunkSize;
	short formatType, channels;
	DWORD sampleRate, avgBytesPerSec;
	short bytesPerSample, bitsPerSample;
	DWORD dataSize;
	////start reading the file and check it
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F')
		return endWithError("No RIFF");

	fread(&size, sizeof(DWORD), 1, fp);
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E')
		return endWithError("No WAVE");

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ')
		return endWithError("no fmt ");

	//read and store the info about wave file
	fread(&chunkSize, sizeof(DWORD), 1, fp);
	fread(&formatType, sizeof(short), 1, fp);
	fread(&channels, sizeof(short), 1, fp);
	fread(&sampleRate, sizeof(DWORD), 1, fp);
	fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
	fread(&bytesPerSample, sizeof(short), 1, fp);
	fread(&bitsPerSample, sizeof(short), 1, fp);

	//make sure you reached the sound data
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a')
		return endWithError("Missing Data ");

	fread(&dataSize, sizeof(DWORD), 1, fp);

	//allocate memory
	unsigned char* buf = new unsigned char[dataSize];
	fread(buf, sizeof(BYTE), dataSize, fp);
	
	ALuint buffer;
	alGenBuffers(1, &buffer);
	ALuint frequency = sampleRate; //?

	ALenum format = 0;
	if(bitsPerSample == 8)
	{
		if (channels == 1)
			format = AL_FORMAT_MONO8;
		else if (channels == 2)
			format = AL_FORMAT_STEREO8;
	}
	else if (bitsPerSample == 16)
	{
		if (channels == 1)
			format = AL_FORMAT_MONO16;
		else if(channels == 2)
			format = AL_FORMAT_STEREO16;
	}
	//load data to buffer
	alBufferData(buffer, format, buf, dataSize, frequency);
	buffers.insert(std::pair<std::string, ALuint>(Filename, buffer));
	fclose(fp);
	delete[] buf; //!?

	return true;
}

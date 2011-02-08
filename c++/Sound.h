#ifndef SOUND_H
#define SOUND_H

#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

class Sound{
public:
	Sound(char *fileName, bool stream);
	~Sound();
	void play();
	void pause();
	void stop();
	void setPosition(int pos);
	void setFrequency(float hz);
	void setPan(float pan);
	void setVolume(float volume);
	int getPosition();
	float getFrequency();
	bool isPlaying();
	int ERRCHECK(FMOD_RESULT result);

	static void setFmodSystem(FMOD::System *fmodSystem);

private:
	FMOD::Sound *sound;
	FMOD::Channel *channel;
	FMOD_RESULT result;

	static FMOD::System *fmodSystem;
};

#endif SOUND_H


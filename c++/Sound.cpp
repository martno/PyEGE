#include "Sound.h"
#include "SoundHandler.h"

#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

#include <iostream>
#include <string>
#include <fstream>

Sound::Sound(char *fileName, bool stream) {
	if(stream) {
		result = fmodSystem->createStream(fileName, FMOD_DEFAULT, 0, &sound);
	}
	else {
		result = fmodSystem->createSound(fileName, FMOD_DEFAULT, 0, &sound);
	}
	ERRCHECK(result);

	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
	ERRCHECK(result);
}

Sound::~Sound() {
	result = sound->release();
	ERRCHECK(result);
}


void Sound::play() {
	result = channel->setPaused(false);
	ERRCHECK(result);

	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
	ERRCHECK(result);
}
void Sound::pause() {
	result = channel->setPaused(true);
	ERRCHECK(result);
}
void Sound::stop() {
	result = channel->stop();
	ERRCHECK(result);
}
void Sound::setPosition(int pos) {
	result = channel->setPosition(pos, FMOD_TIMEUNIT_MS);
	ERRCHECK(result);
}
void Sound::setFrequency(float hz) {
	result = channel->setFrequency(hz);
	ERRCHECK(result);
}
void Sound::setPan(float pan) {
	result = channel->setPan(pan);
	ERRCHECK(result);
}
void Sound::setVolume(float volume) {
	result = channel->setVolume(volume);
	ERRCHECK(result);
}
int Sound::getPosition() {
	unsigned int *pos;
	result = channel->getPosition(pos, FMOD_TIMEUNIT_MS);
	ERRCHECK(result);
	return *pos;
}
float Sound::getFrequency() {
	float *hz;
	result = channel->getFrequency(hz);
	ERRCHECK(result);
	return *hz;
}
bool Sound::isPlaying() {
	bool *isplaying;
	result = channel->isPlaying(isplaying);
	ERRCHECK(result);
	return *isplaying;
}

int Sound::ERRCHECK(FMOD_RESULT result) {
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return -1;
	}
}


void Sound::setFmodSystem(FMOD::System *newFmodSystem) {
	fmodSystem = newFmodSystem;
}

FMOD::System *Sound::fmodSystem = NULL;

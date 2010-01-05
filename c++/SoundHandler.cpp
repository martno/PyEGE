#include "SoundHandler.h"
#include "Sound.h"

#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

#include <iostream>
#include <string>
#include <fstream>


#define MAX_FMOD_CHANNELS 1024


SoundHandler::SoundHandler() {
    /*
        Create a System object and initialize.
    */
    result = FMOD::System_Create(&fmodSystem);
	ERRCHECK(result);
    
    result = fmodSystem->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        //return 0;
    }
    
    result = fmodSystem->getNumDrivers(&numdrivers);
    ERRCHECK(result);

    if (numdrivers == 0)
    {
        result = fmodSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
        ERRCHECK(result);
    }
    else
    {
        result = fmodSystem->getDriverCaps(0, &caps, 0, 0, &speakermode);
        ERRCHECK(result);

        result = fmodSystem->setSpeakerMode(speakermode);       /* Set the user selected speaker mode. */
        ERRCHECK(result);

        if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
        {                                                   /* You might want to warn the user about this. */
            result = fmodSystem->setDSPBufferSize(1024, 10);
            ERRCHECK(result);
        }

        result = fmodSystem->getDriverInfo(0, name, 256, 0);
        ERRCHECK(result);

        if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
        {
            result = fmodSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
            ERRCHECK(result);
        }
    }

    result = fmodSystem->init(MAX_FMOD_CHANNELS, FMOD_INIT_NORMAL, 0);
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
    {
        result = fmodSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        ERRCHECK(result);
            
        result = fmodSystem->init(100, FMOD_INIT_NORMAL, 0);/* ... and re-init. */
        ERRCHECK(result);
    }

	Sound::setFmodSystem(fmodSystem);
}


SoundHandler::~SoundHandler() {
	fmodSystem->release();
}

void SoundHandler::update() {
	fmodSystem->update();
}



int SoundHandler::ERRCHECK(FMOD_RESULT result) {	// I don't think this is how it should be used...
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return -1;
	}
}

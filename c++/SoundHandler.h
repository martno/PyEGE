#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

class SoundHandler {
public:
	SoundHandler();
	~SoundHandler();

	void update();

private:
	/* Private Methods: */
	int ERRCHECK(FMOD_RESULT result);

	/* Private Variables: */
	FMOD::System    *fmodSystem;
	FMOD_RESULT      result;
	unsigned int     version;
	int              numdrivers;
	FMOD_SPEAKERMODE speakermode;
	FMOD_CAPS        caps;
	char             name[256];

};

#include "system/audio.h"

void lycoris::system::audio::audio_system::initialize()
{
	XAudio2Create(x_audio_.put());
}

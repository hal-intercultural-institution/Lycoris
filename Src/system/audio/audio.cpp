#include "system/audio.h"

void lycoris::system::audio::audio_system::initialize()
{
	XAudio2Create(&x_audio_);
	x_audio_->CreateMasteringVoice(mastering_voice_.put());
}

void lycoris::system::audio::audio_system::destroy()
{
	x_audio_->Release();
}

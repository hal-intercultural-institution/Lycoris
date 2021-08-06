#include "system/audio.h"

void lycoris::system::audio::audio_system::initialize()
{
	HRESULT hr = XAudio2Create(&x_audio_);
	if (FAILED(hr))
		throw std::runtime_error("AudioSystem: failed to initialize XAudio2");

	hr = x_audio_->CreateMasteringVoice(mastering_voice_.put());
	if (FAILED(hr))
		throw std::runtime_error("AudioSystem: failed to initialize Mastering Voice");

}

void lycoris::system::audio::audio_system::destroy()
{
	x_audio_->Release();
}

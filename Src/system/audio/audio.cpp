#include "system/audio.h"

#include "system/audioloader.h"
#include "utility/cast.h"

using lycoris::utility::scast::uint32_of;

IXAudio2SourceVoice& lycoris::system::audio::sound::get_voice()
{
	return *source_voice_;
}

lycoris::system::audio::wav_file& lycoris::system::audio::sound::get_file()
{
	return file_;
}

void lycoris::system::audio::audio_system::initialize()
{
	HRESULT hr = XAudio2Create(&x_audio_);
	if (FAILED(hr))
		throw std::runtime_error("AudioSystem: failed to initialize XAudio2");

	hr = x_audio_->CreateMasteringVoice(&mastering_voice_);
	if (FAILED(hr))
		throw std::runtime_error("AudioSystem: failed to initialize Mastering Voice");

}

void lycoris::system::audio::audio_system::destroy()
{
	mastering_voice_->DestroyVoice();
	x_audio_->Release();
}

lycoris::system::audio::sound lycoris::system::audio::audio_system::load_sound_from_file(std::filesystem::path& path) const
{
	const auto extension = path.extension().string();
	wav_file wav_file;
	utility::xaudio2_voice<IXAudio2SourceVoice> voice;
	if (extension == ".wav" || extension == ".wave")
	{
		wav_file = wav_loader::load_from_file(path);
	}
	HRESULT hr = x_audio_->CreateSourceVoice(voice.put(), &wav_file.format);
	if (FAILED(hr))
	{
		throw std::runtime_error("AudioSystem: failed to create source voice");
	}
	
	return sound(std::move(voice), std::move(wav_file));
}

void lycoris::system::audio::audio_system::play(sound& sound, std::uint32_t time, float volume)
{
	IXAudio2SourceVoice& voice = sound.get_voice();
	wav_file& file = sound.get_file();

	XAUDIO2_VOICE_STATE state;
	voice.GetState(&state);
	if (state.BuffersQueued != 0)
	{
		voice.Stop();
		voice.FlushSourceBuffers();
	}

	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = reinterpret_cast<BYTE*>(file.buffer.data());
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = uint32_of(file.buffer.size());
	buffer.LoopCount = time;
	
	HRESULT hr = voice.SubmitSourceBuffer(&buffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("AudioSystem: failed to submit buffer");
	}

	hr = voice.SetVolume(volume);
	if (FAILED(hr))
	{
		throw std::runtime_error("AudioSystem: failed to set volume");
	}

	voice.Start();
}

void lycoris::system::audio::audio_system::play(sound& sound, std::uint32_t time)
{
	play(sound, time, 1.0f);
}

void lycoris::system::audio::audio_system::play(sound& sound, float volume)
{
	play(sound, 1, volume);
}

void lycoris::system::audio::audio_system::play_looped(sound& sound)
{
	play(sound, XAUDIO2_LOOP_INFINITE, 1.0f);
}

void lycoris::system::audio::audio_system::play_looped(sound& sound, float volume)
{
	play(sound, XAUDIO2_LOOP_INFINITE, volume);
}

void lycoris::system::audio::audio_system::stop(sound& sound)
{
	IXAudio2SourceVoice& voice = sound.get_voice();
	
	XAUDIO2_VOICE_STATE state;
	voice.GetState(&state);
	if (state.BuffersQueued != 0)
	{
		voice.Stop();
		voice.FlushSourceBuffers();
	}
}
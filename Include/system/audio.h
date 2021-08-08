#pragma once

#include <filesystem>

#include <winrt/base.h>
#include <xaudio2.h>

#include "utility/xaudio2voice.h"

namespace lycoris::system::audio
{
	class wav_file
	{
	public:
		wav_file(WAVEFORMATEX format, std::vector<std::byte>&& buffer)
			: format(format), buffer(std::move(buffer)) {}
		wav_file() : format({}), buffer(0) {}

		wav_file(const wav_file&) = delete;
		wav_file(wav_file&&) = default;

		wav_file& operator=(const wav_file&) = delete;
		wav_file& operator=(wav_file&&) = default;

		~wav_file() = default;
		
		WAVEFORMATEX format;
		std::vector<std::byte> buffer;
	};
	
	class sound
	{
	public:
		explicit sound(utility::xaudio2_voice<IXAudio2SourceVoice>&& voice, wav_file&& file)
			: source_voice_(std::move(voice)), file_(std::move(file)) {}
		sound() = default;

		sound(const sound&) = delete;
		sound(sound&&) = default;
		~sound() = default;
		
		sound& operator=(const sound&) = delete;
		sound& operator=(sound&&) = default;
		
		IXAudio2SourceVoice& get_voice();
		wav_file& get_file();
	
	private:
		utility::xaudio2_voice<IXAudio2SourceVoice> source_voice_;
		wav_file file_;
	};
	
	class audio_system
	{
	public:
		void initialize();
		void destroy();
		sound load_sound_from_file(std::filesystem::path& path) const;
		static void play(sound& sound, std::uint32_t time, float volume);
		static void play(sound& sound, std::uint32_t time);
		static void play(sound& sound, float volume);
		static void play_looped(sound& sound);
		static void play_looped(sound& sound, float volume);
		static void stop(sound& sound);
	
	private:
		//winrt::com_ptr<IXAudio2> x_audio_;
		IXAudio2* x_audio_ = nullptr;
		/*utility::xaudio2_voice<IXAudio2MasteringVoice> mastering_voice_;*/
		IXAudio2MasteringVoice* mastering_voice_ = nullptr;
	};
}

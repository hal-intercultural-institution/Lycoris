#pragma once

#include <winrt/base.h>
#include <xaudio2.h>

#include "utility/xaudio2voice.h"

namespace lycoris::system::audio
{	
	class audio_system
	{
	public:
		void initialize();
	private:
		winrt::com_ptr<IXAudio2> x_audio_;
		utility::xaudio2_voice<IXAudio2MasteringVoice> mastering_voice_;
	};
}
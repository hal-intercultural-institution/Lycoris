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
		void destroy();
	private:
		//winrt::com_ptr<IXAudio2> x_audio_;
		IXAudio2* x_audio_ = nullptr;
		utility::xaudio2_voice<IXAudio2MasteringVoice> mastering_voice_;
	};
}
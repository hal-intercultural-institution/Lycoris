#pragma once

#include <winrt/base.h>
#include <xaudio2.h>

namespace lycoris::system::audio
{	
	class audio_system
	{
	public:
		void initialize();
	private:
		winrt::com_ptr<IXAudio2> x_audio_;
	};
}
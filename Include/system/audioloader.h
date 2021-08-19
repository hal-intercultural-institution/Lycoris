#pragma once

#include <filesystem>

#include "system/audio.h"

namespace lycoris::system::audio
{	
	class wav_loader
	{
	public:
		static wav_file load_from_file(const std::filesystem::path& path);
	};
}

#pragma once

#include <cstdint>

namespace lycoris::system
{
	class settings
	{
	public:
		std::uint16_t screen_width;
		std::uint16_t screen_height;

		settings();
	};
}

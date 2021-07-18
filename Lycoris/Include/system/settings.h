#pragma once

#include <cstdint>
#include <string_view>

namespace lycoris::system
{
	class settings
	{
	public:
		std::uint16_t screen_width;
		std::uint16_t screen_height;

		// default constructor, initialize values with default
		settings();
		// initialize values by specified setting file
		settings(std::string_view file);
	};
}

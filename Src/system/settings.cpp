#pragma once

#include "system/settings.h"

lycoris::system::settings::settings()
{
	screen_width = 1280;
	screen_height = 720;
}

lycoris::system::settings::settings(std::string_view file)
{
	screen_width = 1280;
	screen_height = 720;
}

#include "render/renderer.h"

void lycoris::render::screen::resize(std::uint32_t new_width, std::uint32_t new_height)
{
	if (screen_width_ == new_width && screen_width_ == new_height) return;
	
	screen_width_ = static_cast<float>(new_width);
	screen_height_ = static_cast<float>(new_height);
}

float lycoris::render::screen::get_screen_width() const
{
	return screen_width_;
}

float lycoris::render::screen::get_screen_height() const
{
	return screen_height_;
}

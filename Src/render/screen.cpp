#include "render/screen.h"

void lycoris::render::screen::resize(const std::uint32_t new_width, const std::uint32_t new_height)
{
	const auto wid = static_cast<float>(new_width), hei = static_cast<float>(new_height);
	if (screen_width_ == wid && screen_width_ == hei) return;

	screen_width_ = wid;
	screen_height_ = hei;
}

void lycoris::render::screen::set_activation(const bool activation) noexcept
{
	activation_ = activation;
}

void lycoris::render::screen::set_window_handle(HWND window_handle)
{
	window_handle_ = window_handle;
}

float lycoris::render::screen::get_screen_width() const
{
	return screen_width_;
}

float lycoris::render::screen::get_screen_height() const
{
	return screen_height_;
}

bool lycoris::render::screen::is_active() const noexcept
{
	return activation_;
}

HWND lycoris::render::screen::get_window_handle() const
{
	return window_handle_;
}

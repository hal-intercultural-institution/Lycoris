#include "system/input.h"

void lycoris::system::input::keyboard::update(RAWKEYBOARD& data)
{
	if (data.Flags & RI_KEY_BREAK)
		key_state_[data.VKey] = false;
	else
		key_state_[data.VKey] = true;
}

void lycoris::system::input::keyboard::post_update()
{
	key_state_previous_ = key_state_;
}

bool lycoris::system::input::keyboard::is_triggered(std::uint8_t key)
{
	return !key_state_previous_[key] && key_state_[key];
}

bool lycoris::system::input::keyboard::is_released(std::uint8_t key)
{
	return key_state_previous_[key] && !key_state_[key];
}

bool lycoris::system::input::keyboard::is_pressed(std::uint8_t key)
{
	return key_state_[key];
}

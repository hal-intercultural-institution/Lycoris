#include "system/input.h"

#include <Windowsx.h>

#include "game.h"

namespace
{
	constexpr auto keycode_left = static_cast<std::uint64_t>(lycoris::system::input::mouse::keycode::left);
	constexpr auto keycode_right = static_cast<std::uint64_t>(lycoris::system::input::mouse::keycode::right);
	constexpr auto keycode_middle = static_cast<std::uint64_t>(lycoris::system::input::mouse::keycode::middle);
	constexpr auto keycode_forward = static_cast<std::uint64_t>(lycoris::system::input::mouse::keycode::forward);
	constexpr auto keycode_back = static_cast<std::uint64_t>(lycoris::system::input::mouse::keycode::back);
	constexpr auto keycode_wheel_up = static_cast<std::uint64_t>(lycoris::system::input::mouse::keycode::wheel_up);
	constexpr auto keycode_wheel_down = static_cast<std::uint64_t>(lycoris::system::input::mouse::keycode::wheel_down);
}

void lycoris::system::input::mouse::update(RAWMOUSE& data)
{
	if (data.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
		button_state_[keycode_left] = true;
	
	else if (data.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
		button_state_[keycode_left] = false;
	
	else if (data.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		button_state_[keycode_right] = true;
	
	else if (data.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
		button_state_[keycode_right] = false;
	
	else if (data.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		button_state_[keycode_middle] = true;
	
	else if (data.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
		button_state_[keycode_middle] = false;

	else if (data.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
		button_state_[keycode_back] = true;

	else if (data.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
		button_state_[keycode_back] = false;

	else if (data.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
		button_state_[keycode_forward] = true;

	else if (data.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
		button_state_[keycode_forward] = false;

	else if (data.usButtonFlags & RI_MOUSE_WHEEL)
	{
		const auto delta = static_cast<std::int16_t>(data.usButtonData);
		if (delta > 0) // positive value = scrolled forward
			button_state_[keycode_wheel_up] = true;
		else if (delta < 0) // negative value = scrolled back
			button_state_[keycode_wheel_down] = true;
	}

	else if (data.usButtonFlags == 0) // no data = mouse movement?
	{
		move_x_ += data.lLastX;
		move_y_ += data.lLastY;
	}
}

void lycoris::system::input::mouse::update()
{
	const auto& screen = game::get_game().get_renderer().get_screen();
	GetCursorPos(&screen_pos_);
	ScreenToClient(screen.get_window_handle(), &screen_pos_);
	is_on_screen_ = !(screen_pos_.x < 0 || screen_pos_.y < 0
		|| screen_pos_.x > static_cast<std::int32_t>(screen.get_screen_width()) - 1
		|| screen_pos_.y > static_cast<std::int32_t>(screen.get_screen_height()) - 1);
}

void lycoris::system::input::mouse::post_update()
{
	button_state_previous_ = button_state_;
	button_state_[keycode_wheel_up] = button_state_[keycode_wheel_down] = false;
	move_x_ = move_y_ = 0;
}

bool lycoris::system::input::mouse::is_triggered(keycode keycode)
{
	const auto keycode_num = static_cast<std::uint64_t>(keycode);
	return !button_state_previous_[keycode_num] && button_state_[keycode_num];
}

bool lycoris::system::input::mouse::is_released(keycode keycode)
{
	const auto keycode_num = static_cast<std::uint64_t>(keycode);
	return button_state_previous_[keycode_num] && !button_state_[keycode_num];
}

bool lycoris::system::input::mouse::is_pressed(keycode keycode)
{
	const auto keycode_num = static_cast<std::uint64_t>(keycode);
	return button_state_[keycode_num];
}

std::int32_t lycoris::system::input::mouse::get_move_x()
{
	return move_x_;
}

std::int32_t lycoris::system::input::mouse::get_move_y()
{
	return move_y_;
}

std::int32_t lycoris::system::input::mouse::get_screen_x()
{
	return screen_pos_.x;
}

std::int32_t lycoris::system::input::mouse::get_screen_y()
{
	return screen_pos_.y;
}

bool lycoris::system::input::mouse::is_on_screen()
{
	return is_on_screen_;
}

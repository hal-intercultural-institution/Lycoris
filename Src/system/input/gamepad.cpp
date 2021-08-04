#include "system/input.h"

bool lycoris::system::input::game_pad::is_triggered(keycode keycode)
{
	const auto keycode_num = static_cast<std::uint64_t>(keycode);
	return !button_state_previous_[keycode_num] && button_state_[keycode_num];
}

bool lycoris::system::input::game_pad::is_released(keycode keycode)
{
	const auto keycode_num = static_cast<std::uint64_t>(keycode);
	return button_state_previous_[keycode_num] && !button_state_[keycode_num];
}

bool lycoris::system::input::game_pad::is_pressed(keycode keycode)
{
	const auto keycode_num = static_cast<std::uint64_t>(keycode);
	return button_state_[keycode_num];
}

std::int32_t lycoris::system::input::game_pad::get_left_x() const
{
	return left_x_;
}

std::int32_t lycoris::system::input::game_pad::get_left_y() const
{
	return left_y_;
}

std::int32_t lycoris::system::input::game_pad::get_right_x() const
{
	return right_x_;
}

std::int32_t lycoris::system::input::game_pad::get_right_y() const
{
	return right_y_;
}

float lycoris::system::input::game_pad::get_left_stick_tilt() const
{
	if (stick_dead_zone_ > stick_tilt_left_) return 0.0f;
	return stick_tilt_left_;
}

float lycoris::system::input::game_pad::get_right_stick_tilt() const
{
	if (stick_dead_zone_ > stick_tilt_right_) return 0.0f;
	return stick_tilt_right_;
}

float lycoris::system::input::game_pad::get_left_stick_angle() const
{
	return stick_angle_left_;
}

float lycoris::system::input::game_pad::get_right_stick_angle() const
{
	return stick_angle_right_;
}

std::int32_t lycoris::system::input::game_pad::get_left_trigger_pressure() const
{
	return trigger_pressure_left_;
}

std::int32_t lycoris::system::input::game_pad::get_right_trigger_pressure() const
{
	return trigger_pressure_right_;
}

float lycoris::system::input::game_pad::get_stick_dead_zone() const
{
	return stick_dead_zone_;
}

void lycoris::system::input::game_pad::set_stick_dead_zone(float dead_zone)
{
	stick_dead_zone_ = dead_zone;
}

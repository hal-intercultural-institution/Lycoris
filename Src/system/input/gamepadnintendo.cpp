#include "system/input.h"

void lycoris::system::input::nintendo_game_pad::update()
{
	// copy prev state
	button_state_previous_ = button_state_;
	// initialize values
	button_state_.fill(false);
	stick_tilt_left_ = stick_tilt_right_ = stick_angle_left_ = stick_angle_right_ = 0.0f;
	trigger_pressure_left_ = trigger_pressure_right_ = 0;

	DIJOYSTATE state;
	device_->Poll();
	HRESULT hr = device_->GetDeviceState(sizeof(DIJOYSTATE), &state);
	if (FAILED(hr))
	{
		device_->Acquire();
		return;
	}

	// ぼたん
	if (state.rgbButtons[0] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::right_south)] = true; // B
	if (state.rgbButtons[1] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::right_east)] = true; // A
	if (state.rgbButtons[2] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::right_west)] = true; // Y
	if (state.rgbButtons[3] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::right_north)] = true; // X
	if (state.rgbButtons[4] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::left_button)] = true; // L
	if (state.rgbButtons[5] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::right_button)] = true; // R
	if (state.rgbButtons[6] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::left_trigger)] = true; // ZL
	if (state.rgbButtons[7] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::right_trigger)] = true; // ZR
	if (state.rgbButtons[8] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::extra_1)] = true; // Minus
	if (state.rgbButtons[9] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::extra_2)] = true; // Plus
	if (state.rgbButtons[10] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::left_joystick)] = true; // LStick
	if (state.rgbButtons[11] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::right_joystick)] = true; // RStick
	if (state.rgbButtons[12] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::extra_4)] = true; // Home
	if (state.rgbButtons[13] & 0x80) button_state_[static_cast<std::uint64_t>(keycode::extra_3)] = true; // Capture

	left_x_ = state.lX;
	left_y_ = state.lY;
	right_x_ = state.lRx;
	right_y_ = state.lRy;
	// Switch Pro Controller doesn't have analog triggers

	stick_tilt_left_ = std::sqrtf(static_cast<float>(left_x_ * left_x_ + left_y_ * left_y_));
	stick_tilt_right_ = std::sqrtf(static_cast<float>(right_x_ * right_x_ + right_y_ * right_y_));

	if (stick_dead_zone_ < stick_tilt_left_)
		stick_angle_left_ = std::atan2f(static_cast<float>(-left_y_), static_cast<float>(left_x_));
	if (stick_dead_zone_ < stick_tilt_right_)
		stick_angle_right_ = std::atan2f(static_cast<float>(-right_y_), static_cast<float>(right_x_));

	// じゅうじきー
	// 0xFFFFFFFFが入っていると押されていない状態
	if (state.rgdwPOV[0] == 0xFFFFFFFF) return;
	switch (state.rgdwPOV[0])
	{
	case 0: // うえ
		button_state_[static_cast<std::uint64_t>(keycode::left_north)] = true;
		break;
	case 4500: // みぎうえ
		button_state_[static_cast<std::uint64_t>(keycode::left_north)] = true;
		button_state_[static_cast<std::uint64_t>(keycode::left_east)] = true;
		break;
	case 9000: // みぎ
		button_state_[static_cast<std::uint64_t>(keycode::left_east)] = true;
		break;
	case 13500: // みぎした
		button_state_[static_cast<std::uint64_t>(keycode::left_east)] = true;
		button_state_[static_cast<std::uint64_t>(keycode::left_south)] = true;
		break;
	case 18000: // した
		button_state_[static_cast<std::uint64_t>(keycode::left_south)] = true;
		break;
	case 22500: // ひだりした
		button_state_[static_cast<std::uint64_t>(keycode::left_south)] = true;
		button_state_[static_cast<std::uint64_t>(keycode::left_west)] = true;
		break;
	case 27000: // ひだり
		button_state_[static_cast<std::uint64_t>(keycode::left_west)] = true;
		break;
	case 31500: // ひだりうえ
		button_state_[static_cast<std::uint64_t>(keycode::left_west)] = true;
		button_state_[static_cast<std::uint64_t>(keycode::left_north)] = true;
		break;
	default:
		break;
	}

}

void lycoris::system::input::nintendo_game_pad::destroy()
{
	device_->Unacquire();
}


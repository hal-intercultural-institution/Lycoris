#pragma once

#include <array>
#include <cstdint>

#include <Windows.h>

namespace lycoris::system::input
{
	class keyboard
	{
	public:
		void update(RAWKEYBOARD& data);
		// saves input state of this frame, for compare states in next frame. must be called after on_tick function(s).
		void post_update();
		bool is_triggered(std::uint8_t key);
		bool is_released(std::uint8_t key);
		bool is_pressed(std::uint8_t key);
	private:
		std::array<bool, 256> key_state_previous_ = {};
		std::array<bool, 256> key_state_ = {};
	};
	
	class input
	{
	public:
		void initialize();
		void update();
		void post_update();
		void update_raw_input(std::int64_t l_param);
		void destroy();
		keyboard& get_keyboard();
	private:
		keyboard keyboard_;
	};
}

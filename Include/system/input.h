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

	class mouse
	{
	public:
		enum class keycode : std::uint32_t
		{
			left,
			right,
			middle,
			forward,
			back,
			wheel_up,
			wheel_down
		};

		// method for process raw input api
		void update(RAWMOUSE& data);
		// method for retrieve on-window cursor position
		void update(std::int64_t l_param);
		void post_update();
		bool is_triggered(keycode keycode);
		bool is_released(keycode keycode);
		bool is_pressed(keycode keycode);
		std::int32_t get_move_x();
		std::int32_t get_move_y();
		std::int32_t get_screen_x();
		std::int32_t get_screen_y();
	
	private:
		std::array<bool, 8> button_state_previous_ = {};
		std::array<bool, 8> button_state_ = {};
		std::int32_t move_x_ = 0;
		std::int32_t move_y_ = 0;
		std::int32_t screen_x_ = 0;
		std::int32_t screen_y_ = 0;
	};
	
	class input
	{
	public:
		void initialize();
		void update();
		void post_update();
		void update_raw_input(std::int64_t l_param);
		void update_mouse_move(std::int64_t l_param);
		void destroy();
		keyboard& get_keyboard();
		mouse& get_mouse();
	
	private:
		keyboard keyboard_;
		mouse mouse_;
	};

}

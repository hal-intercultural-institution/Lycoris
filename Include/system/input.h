#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <array>
#include <cstdint>

#include <Windows.h>
#include <dinput.h>
#include <winrt/base.h>

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
		void update();
		void post_update();
		bool is_triggered(keycode keycode);
		bool is_released(keycode keycode);
		bool is_pressed(keycode keycode);
		std::int32_t get_move_x();
		std::int32_t get_move_y();
		std::int32_t get_screen_x();
		std::int32_t get_screen_y();
		bool is_on_screen();
	
	private:
		std::array<bool, 8> button_state_previous_ = {};
		std::array<bool, 8> button_state_ = {};
		std::int32_t move_x_ = 0;
		std::int32_t move_y_ = 0;
		POINT screen_pos_{};
		bool is_on_screen_ = false;
	};

	class game_pad
	{
	public:
		enum class keycode
		{
			left_west,
			left_south,
			left_east,
			left_north,
			
			right_west,
			right_south,
			right_east,
			right_north,
			
			left_button,
			right_button,
			
			left_trigger,
			right_trigger,

			left_joystick,
			right_joystick,

			extra_1,
			extra_2,
			extra_3,
			extra_4
		};
		
		virtual void update() = 0;
		bool is_triggered(keycode keycode);
		bool is_released(keycode keycode);
		bool is_pressed(keycode keycode);
		std::int32_t get_left_x() const;
		std::int32_t get_left_y() const;
		std::int32_t get_right_x() const;
		std::int32_t get_right_y() const;
		float get_left_stick_tilt() const;
		float get_right_stick_tilt() const;
		float get_left_stick_angle() const;
		float get_right_stick_angle() const;
		std::int32_t get_left_trigger_pressure() const;
		std::int32_t get_right_trigger_pressure() const;
		float get_stick_dead_zone() const;
		void set_stick_dead_zone(float dead_zone);
		virtual void destroy() = 0;
		virtual ~game_pad() = default;
	protected:
		std::array<bool, 18> button_state_previous_{};
		std::array<bool, 18> button_state_{};
		std::int32_t left_x_ = 0;
		std::int32_t left_y_ = 0;
		std::int32_t right_x_ = 0;
		std::int32_t right_y_ = 0;
		std::int32_t trigger_pressure_left_ = 0;
		std::int32_t trigger_pressure_right_ = 0;
		float stick_tilt_left_ = 0.0f;
		float stick_tilt_right_ = 0.0f;
		float stick_angle_left_ = 0.0f;
		float stick_angle_right_ = 0.0f;
		float stick_dead_zone_ = 100.0f;
	};

	class empty_game_pad final : public game_pad
	{
		void update() override;
		void destroy() override;
	};

	class sony_game_pad final : public game_pad
	{
	public:
		sony_game_pad() = default;
		explicit sony_game_pad(winrt::com_ptr<IDirectInputDevice8> && ptr)
		{
			device_ = std::move(ptr);
		}
		sony_game_pad(const sony_game_pad&) = delete;
		sony_game_pad(sony_game_pad&&) = default;
		~sony_game_pad() override = default;

		sony_game_pad& operator=(const sony_game_pad&) = delete;
		sony_game_pad& operator=(sony_game_pad&&) = default;

		void update() override;
		void destroy() override;
	
	private:
		inline static std::uint64_t count_ = 0;
		winrt::com_ptr<IDirectInputDevice8> device_;
	};

	class nintendo_game_pad final : public game_pad
	{
	public:
		nintendo_game_pad() = default;
		explicit nintendo_game_pad(winrt::com_ptr<IDirectInputDevice8>&& ptr)
		{
			device_ = std::move(ptr);
		}
		nintendo_game_pad(const nintendo_game_pad&) = delete;
		nintendo_game_pad(nintendo_game_pad&&) = default;
		~nintendo_game_pad() override = default;

		nintendo_game_pad& operator=(const nintendo_game_pad&) = delete;
		nintendo_game_pad& operator=(nintendo_game_pad&&) = default;

		void update() override;
		void destroy() override;

	private:
		inline static std::uint64_t count_ = 0;
		winrt::com_ptr<IDirectInputDevice8> device_;
	};
	
	class input
	{
	public:
		void initialize();
		void update();
		void post_update();
		void update_raw_input(std::int64_t l_param);
		void destroy();
		void register_direct_input_device(LPCDIDEVICEINSTANCE instance, void* ref);
		keyboard& get_keyboard();
		mouse& get_mouse();
		game_pad& get_game_pad(std::uint64_t index);
		std::uint64_t get_game_pad_count();
	
	private:
		keyboard keyboard_;
		mouse mouse_;
		std::vector<std::unique_ptr<game_pad>> game_pads_;
		std::unique_ptr<game_pad> empty_pad_;
		
		winrt::com_ptr<IDirectInput8> direct_input_;
	};

}

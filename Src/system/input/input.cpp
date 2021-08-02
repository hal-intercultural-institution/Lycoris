#include "system/input.h"

#include <array>
#include <format>
#include <stdexcept>
#include <vector>

#include "utility/cast.h"

using lycoris::utility::scast::uint32_of;

void lycoris::system::input::input::initialize()
{
	std::array<RAWINPUTDEVICE, 2> devices = {
		{
			{ 1, 2, 0, nullptr }, // mouse (2)
			{ 1, 6, 0, nullptr }  // keyboard (6)
		}
	};

	if (!RegisterRawInputDevices(devices.data(), uint32_of(devices.size()), uint32_of(sizeof(RAWINPUTDEVICE))))
	{
		const auto error = GetLastError();
		const auto str = std::format(
			"InputSystem: failed to initialize raw input devices: {x}",
			error
		);
		throw std::runtime_error(str);
	}

}

void lycoris::system::input::input::update()
{

}

void lycoris::system::input::input::post_update()
{
	keyboard_.post_update();
	mouse_.post_update();
}

void lycoris::system::input::input::update_raw_input(std::int64_t l_param)
{
	std::uint32_t buffer_size = 0;
	const auto h_raw_input = reinterpret_cast<HRAWINPUT>(l_param);
	
	GetRawInputData(h_raw_input, RID_INPUT, nullptr, &buffer_size, sizeof(RAWINPUTHEADER));
	std::vector<std::byte> buffer(buffer_size);
	GetRawInputData(h_raw_input, RID_INPUT, buffer.data(), &buffer_size, sizeof(RAWINPUTHEADER));

	auto input = reinterpret_cast<RAWINPUT*>(buffer.data());

	if (input->header.dwType == RIM_TYPEKEYBOARD)
	{
		keyboard_.update(input->data.keyboard);
	}
	else if (input->header.dwType == RIM_TYPEMOUSE)
	{
		mouse_.update(input->data.mouse);
	}
}

void lycoris::system::input::input::update_mouse_move(std::int64_t l_param)
{
	mouse_.update(l_param);
}

void lycoris::system::input::input::destroy()
{
	
}

lycoris::system::input::keyboard& lycoris::system::input::input::get_keyboard()
{
	return keyboard_;
}

lycoris::system::input::mouse& lycoris::system::input::input::get_mouse()
{
	return mouse_;
}

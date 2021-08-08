#include "system/input.h"

#include <array>
#include <format>
#include <stdexcept>
#include <vector>

#include "utility/cast.h"
#include "game.h"

using lycoris::utility::scast::uint32_of;

constexpr auto joystick_range_max = 1000;
constexpr auto joystick_range_min = -1000;
constexpr auto trigger_pressure_min = 0;
constexpr auto trigger_pressure_max = 1000;

BOOL CALLBACK device_callback(LPCDIDEVICEINSTANCE instance, LPVOID ref)
{
	lycoris::game::get_game().get_input_system().register_direct_input_device(instance, ref);
	return DIENUM_CONTINUE;
}

void lycoris::system::input::input::initialize()
{
	auto& game = game::get_game();
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

	if (FAILED(DirectInput8Create(game.get_instance_handle(), DIRECTINPUT_VERSION,
		IID_IDirectInput8, direct_input_.put_void(), nullptr)))
		throw std::runtime_error("InputSystem: failed to initialize direct input");

	direct_input_->EnumDevices(DI8DEVTYPE_GAMEPAD, device_callback, nullptr, DIEDFL_ATTACHEDONLY);
}

void lycoris::system::input::input::update()
{
	for (auto& game_pad : game_pads_)
	{
		game_pad->update();
	}
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
	for (auto& game_pad : game_pads_)
	{
		game_pad->destroy();
	}
	game_pads_.clear();
}

lycoris::system::input::keyboard& lycoris::system::input::input::get_keyboard()
{
	return keyboard_;
}

lycoris::system::input::mouse& lycoris::system::input::input::get_mouse()
{
	return mouse_;
}

lycoris::system::input::game_pad& lycoris::system::input::input::get_game_pad(std::uint64_t index)
{
	if (game_pads_.size() < index)
		throw std::out_of_range("Gamepad: index exceeds count of currently attached game-pads");
	
	return *game_pads_[index];
}

std::uint64_t lycoris::system::input::input::get_game_pad_count()
{
	return game_pads_.size();
}

void lycoris::system::input::input::register_direct_input_device(LPCDIDEVICEINSTANCE instance, void* ref)
{
	winrt::com_ptr<IDirectInputDevice8> device;

	HRESULT hr = direct_input_->CreateDevice(instance->guidInstance, device.put(), nullptr);
	if (FAILED(hr))
		throw std::runtime_error("InputSystem: failed to register direct input device");

	hr = device->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(hr))
		throw std::runtime_error("InputSystem: failed to set data format");

	const auto window_handle = game::get_game().get_renderer().get_screen().get_window_handle();
	hr = device->SetCooperativeLevel(window_handle, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr))
		throw std::runtime_error("InputSystem: failed to set cooperative level");

	DIPROPDWORD data{};
	data.diph.dwHeaderSize = sizeof(data.diph);
	data.diph.dwSize = sizeof(data);
	data.diph.dwHow = DIPH_DEVICE;
	device->GetProperty(DIPROP_VIDPID, &data.diph);

	const auto vendor_id = static_cast<std::uint16_t>(data.dwData);
	const auto device_id = static_cast<std::uint16_t>(data.dwData >> 16);
	
	// joystick / trigger range
	{
		DIPROPRANGE prop;
		prop.diph.dwSize = sizeof(prop);
		prop.diph.dwHeaderSize = sizeof(prop.diph);
		prop.diph.dwHow = DIPH_BYOFFSET;
		prop.lMax = joystick_range_max;
		prop.lMin = joystick_range_min;

		if (vendor_id == 0x057E) // Nintendo
		{
			prop.diph.dwObj = DIJOFS_X; // left x
			device->SetProperty(DIPROP_RANGE, &prop.diph);

			prop.diph.dwObj = DIJOFS_Y; // left y
			device->SetProperty(DIPROP_RANGE, &prop.diph);

			prop.diph.dwObj = DIJOFS_RX; // right x
			device->SetProperty(DIPROP_RANGE, &prop.diph);

			prop.diph.dwObj = DIJOFS_RY; // right y
			device->SetProperty(DIPROP_RANGE, &prop.diph);
		}
		else if (vendor_id == 0x054C) // Sony
		{
			prop.diph.dwObj = DIJOFS_X; // left x
			device->SetProperty(DIPROP_RANGE, &prop.diph);

			prop.diph.dwObj = DIJOFS_Y; // left y
			device->SetProperty(DIPROP_RANGE, &prop.diph);

			prop.diph.dwObj = DIJOFS_Z; // right x
			device->SetProperty(DIPROP_RANGE, &prop.diph);

			prop.diph.dwObj = DIJOFS_RZ; // right y
			device->SetProperty(DIPROP_RANGE, &prop.diph);

			prop.lMax = trigger_pressure_max;
			prop.lMin = trigger_pressure_min;

			prop.diph.dwObj = DIJOFS_RX; // l trigger
			device->SetProperty(DIPROP_RANGE, &prop.diph);

			prop.diph.dwObj = DIJOFS_RY; // r trigger
			device->SetProperty(DIPROP_RANGE, &prop.diph);
		}
		else
		{
			return; // unsupported
		}

	}

	// joystick dead zone
	{
		DIPROPDWORD prop;
		prop.diph.dwSize = sizeof(prop);
		prop.diph.dwHeaderSize = sizeof(prop.diph);
		prop.diph.dwHow = DIPH_BYOFFSET;
		prop.dwData = 0; // 0 value (ここではデッドゾーンの処理はしない)

		prop.diph.dwObj = DIJOFS_X;
		device->SetProperty(DIPROP_DEADZONE, &prop.diph);

		prop.diph.dwObj = DIJOFS_Y;
		device->SetProperty(DIPROP_DEADZONE, &prop.diph);

		prop.diph.dwObj = DIJOFS_Z;
		device->SetProperty(DIPROP_DEADZONE, &prop.diph);

		prop.diph.dwObj = DIJOFS_RZ;
		device->SetProperty(DIPROP_DEADZONE, &prop.diph);

		prop.diph.dwObj = DIJOFS_RX;
		device->SetProperty(DIPROP_DEADZONE, &prop.diph);

		prop.diph.dwObj = DIJOFS_RY;
		device->SetProperty(DIPROP_DEADZONE, &prop.diph);
	}

	device->Acquire();

	std::unique_ptr<game_pad> game_pad = nullptr;
	
	if (vendor_id == 0x057E) game_pad = std::make_unique<nintendo_game_pad>(std::move(device));
	else if (vendor_id == 0x054C) game_pad = std::make_unique<sony_game_pad>(std::move(device));
	
	game_pads_.push_back(std::move(game_pad));
}

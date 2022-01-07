#include "game.h"

#include <stdexcept>

#include <Windows.h>

#include "render/camera.h"
#include "utility/timer.h"
#include "render/renderer.h"

LRESULT CALLBACK wnd_proc(HWND, UINT, WPARAM, LPARAM);

namespace
{
	// pointer of game instance
	// DO NOT CALL delete AGAINST THIS!!!!!!!!!!!!!!!!!!!!!!!!
	lycoris::game::game* instance = nullptr;
	bool initialized = false;

	constexpr auto frame_limit = 60.0;
	constexpr auto window_title = "test";
	constexpr auto class_name = "AppClass";
}

void lycoris::game::game::initialize(HINSTANCE h_instance, int n_show_cmd, MSG* message)
{
	if (instance) 
		throw std::runtime_error("Another instance is already running");
	
	if (!QueryPerformanceFrequency(&utility::timer::frequency))
		throw std::runtime_error("This system not supports performance counter.");

	set_game(this);
	
	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		wnd_proc,
		0,
		0,
		h_instance,
		nullptr,
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		class_name,
		nullptr
	};

	// COM Init
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
	{
		std::exit(EXIT_FAILURE);
	}

	// windows class
	RegisterClassEx(&wcex);

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	const HWND h_wnd = CreateWindow(class_name,
		window_title,
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		settings_.screen_width,
		settings_.screen_height,
		nullptr,
		nullptr,
		h_instance,
		nullptr);

	RECT window_size{}, client_size{};
	GetWindowRect(h_wnd, &window_size);
	GetClientRect(h_wnd, &client_size);
	window_border_x_ = window_size.right - window_size.left - (client_size.right - client_size.left);
	window_border_y_ = window_size.bottom - window_size.top - (client_size.bottom - client_size.top);

	SetWindowPos(h_wnd, nullptr, CW_USEDEFAULT, CW_USEDEFAULT,
		window_border_x_ + settings_.screen_width, window_border_y_ + settings_.screen_height, SWP_NOMOVE);

	ShowWindow(h_wnd, n_show_cmd);
	UpdateWindow(h_wnd);

	h_instance_ = h_instance;
	message_ = message;
	
	try
	{
		logger = profiler::logger("log.txt");
		renderer_.initialize(h_instance, h_wnd, true);
		for (auto& camera : renderer_.get_cameras())
		{
			camera.initialize();
		}
		texture_loader_.initialize();
		input_system_.initialize();
		audio_system_.initialize();
		overlay_.initialize();
		default_viewport_ = render::viewport(1.0f, 1.0f, 0.0f, 0.0f);
	}
	catch (const std::runtime_error& e)
	{
		MessageBox(h_wnd, e.what(), "Game Initialization Failed", MB_ICONSTOP);
		std::exit(EXIT_FAILURE);
	}
	initialized = true;
	renderer_.get_screen().set_activation(true);
}

void lycoris::game::game::run()
{
	while (true)
	{
		utility::timer tick_timer;
		tick_timer.start();
		if (PeekMessage(message_, nullptr, 0, 0, PM_REMOVE))
		{
			if (message_->message == WM_QUIT) return;
			TranslateMessage(message_);
			DispatchMessage(message_);
		}
		else
		{
			static uint32_t temp_frame_count;
			if (per_second_.QuadPart == 0)
				QueryPerformanceCounter(&per_second_);

			utility::timer frame_limiter, draw_timer;
			frame_limiter.start();

			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);
			if (now.QuadPart - per_second_.QuadPart >= utility::timer::frequency.QuadPart)
			{
				QueryPerformanceCounter(&per_second_);
				frames_last_second_ = temp_frame_count;
				temp_frame_count = 0;
			}

			++frame_count_;
			++temp_frame_count;

			//tick_timer.start();
			on_tick();
			frame_time_tick_ = tick_timer.stop();

			draw_timer.start();
			on_draw();
			frame_time_draw_ = draw_timer.stop();

			// frame time (second)
			const auto frame_time = frame_limiter.stop();

			if (constexpr auto max_time = 1.0 / frame_limit; frame_time <= max_time)
			{
				timeBeginPeriod(1);
				Sleep(static_cast<std::uint32_t>((max_time - frame_time) * 1000.0));
				timeEndPeriod(1);
			}
		}
	}
}

void lycoris::game::game::on_tick()
{
	input_system_.update();
	if (scene_) scene_->on_tick();
	if (load_screen_)
	{
		switch (load_screen_->get_load_status())
		{
		case load_screen::status::transition_in:
			load_screen_->on_tick();
			break;
		case load_screen::status::loading:
			if (scene_) scene_->on_destroy();
			scene_ = std::move(next_scene_);
			scene_->on_initialize();
			load_screen_->set_load_status(load_screen::status::transition_out);
			break;
		case load_screen::status::transition_out:
			load_screen_->on_tick();
			break;
		default:
			break;
		}
	}

	for (auto& camera : renderer_.get_cameras())
	{
		if (!camera) continue;
		camera.on_tick();
	}
	overlay_.on_tick();

	if (input_system_.get_keyboard().is_triggered(VK_F11))
	{
		renderer_.get_screen().set_fullscreen(!renderer_.get_screen().is_fullscreen());
	}

	input_system_.post_update();
}

void lycoris::game::game::on_draw()
{
	renderer_.clear();
	internal_draw_calls_ = 0;

	if (scene_)
	{
		std::size_t index = 0;
		for (auto& camera : renderer_.get_cameras())
		{
			if (!camera) continue;
			renderer_.set_current_camera(index);
			camera.set();
			scene_->on_draw();
			index++;
		}
	}
	renderer_.set_viewport(default_viewport_);
	if (scene_) scene_->on_overlay();

	if (load_screen_ && load_screen_->get_load_status() != load_screen::status::not_loading)
		load_screen_->on_draw();
	overlay_.on_draw();

	renderer_.present();
}

void lycoris::game::game::destroy()
{
	// デストラクタよりあとに解放されると都合が悪い
	if (scene_)
	{
		scene_->on_destroy();
		delete scene_.release();
	}
	if (load_screen_)
	{
		load_screen_->on_destroy();
		delete load_screen_.release();
	}
	
	renderer_.destroy();
	texture_loader_.destroy();
	input_system_.destroy();
	audio_system_.destroy();
	
	UnregisterClass(class_name, h_instance_);
}

void lycoris::game::game::increment_draw_call_count() noexcept
{
	internal_draw_calls_++;
}

bool lycoris::game::game::is_initialized() noexcept
{
	return initialized;
}

lycoris::game::game::~game() noexcept
{
	CoUninitialize();
}

std::uint64_t lycoris::game::game::get_frame_count() const noexcept
{
	return frame_count_;
}

std::uint32_t lycoris::game::game::get_fps_last_second() const noexcept
{
	return frames_last_second_;
}

double lycoris::game::game::get_last_tick_time() const noexcept
{
	return frame_time_tick_;
}

double lycoris::game::game::get_last_draw_time() const noexcept
{
	return frame_time_draw_;
}

std::uint32_t lycoris::game::game::get_draw_call_count() const noexcept
{
	return internal_draw_calls_;
}

std::int32_t lycoris::game::game::get_window_border_x() const noexcept
{
	return window_border_x_;
}

std::int32_t lycoris::game::game::get_window_border_y() const noexcept
{
	return window_border_y_;
}

HINSTANCE lycoris::game::game::get_instance_handle() const noexcept
{
	return h_instance_;
}

const lycoris::system::settings& lycoris::game::game::get_launch_settings() const noexcept
{
	return settings_;
}

lycoris::game::scene& lycoris::game::game::get_current_scene() const
{
	if (!scene_)
		throw std::runtime_error("Scene: scene_ is nullptr.");
	return *scene_;
}

void lycoris::game::game::set_scene(std::unique_ptr<scene>&& scene)
{
	load_screen_->set_load_status(load_screen::status::transition_in);
	next_scene_ = std::move(scene);
}

void lycoris::game::game::set_load_screen(std::unique_ptr<load_screen>&& load_screen)
{
	load_screen_ = std::move(load_screen);
	load_screen_->on_initialize();
}

void lycoris::game::game::set_window_title(const char* title)
{
	SetWindowText(renderer_.get_screen().get_window_handle(), title);
}

void lycoris::game::game::set_settings(const system::settings& settings)
{
	settings_ = settings;
}

lycoris::render::renderer& lycoris::game::game::get_renderer() noexcept
{
	return renderer_;
}

lycoris::render::texture::texture_loader& lycoris::game::game::get_texture_loader() noexcept
{
	return texture_loader_;
}

lycoris::system::input::input& lycoris::game::game::get_input_system() noexcept
{
	return input_system_;
}

lycoris::system::audio::audio_system& lycoris::game::game::get_audio_system() noexcept
{
	return audio_system_;
}

lycoris::game::game& lycoris::game::get_game() noexcept
{
	return *instance;
}

void lycoris::game::game::set_game(game* ptr)
{
	instance = ptr;
}

// window procedure
LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		// ウィンドウ破棄操作 (タイトルのバツ、Alt+F4)など を拾って、
		PostQuitMessage(0);
		break;
	case WM_SYSKEYDOWN:
		break;
	case WM_INPUT:
		lycoris::game::get_game().get_input_system().update_raw_input(lParam);
		break;
	case WM_SIZE:
		if (!lycoris::game::game::is_initialized()) break;
		if (lycoris::game::get_game().get_renderer().get_screen().is_fullscreen()) break;
		switch (wParam)
		{
		case 0: // restored, maximized
		case 2:
			lycoris::game::get_game().get_renderer().get_screen().resize(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF);
			break;
		default: // minimized, max show, max hide
			break;
		}
		break;
	case WM_ACTIVATE:
		if (!lycoris::game::game::is_initialized()) break;
		lycoris::game::get_game().get_renderer().get_screen().set_activation(LOWORD(wParam) != WA_INACTIVE);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

#include "profiler/overlay.h"

#include <format>

#include "game.h"
#include "constants.h"

void lycoris::profiler::debug_overlay::initialize()
{
	const auto& renderer = game::get_game().get_renderer();
	format_ = renderer.create_text_format(L"Consolas", 16.0f);
	color_ = renderer.create_text_color({ 1.0f, 1.0f, 1.0f, 1.0f });
	canvas_ = render::text_canvas(0.0f, 0.0f, 600.0f, 400.0f);
}

void lycoris::profiler::debug_overlay::on_tick()
{
	if (game::get_game().get_input_system().get_keyboard().is_triggered(VK_F3))
		shown_ = !shown_;
	if (!shown_) return;

	auto& game = game::get_game();
	text_ = std::format(
		L"Lycoris {}-{}, {} build\nfps: {}\nTick: {:f}\nDraw: {:f}\nMouse x: {}\nMouse y: {}\nGame-pads: {}\nCamera pos: {} {} {}\nCamera rot: {} {}\nCamera distance: {}",
		constants::branch_name,
		constants::commit_id,
		constants::configuration_name,
		game.get_fps_last_second(),
		game.get_last_tick_time(),
		game.get_last_draw_time(),
		game.get_input_system().get_mouse().get_screen_x(),
		game.get_input_system().get_mouse().get_screen_y(),
		game.get_input_system().get_game_pad_count(),
		game.get_renderer().get_camera().get_position().x,
		game.get_renderer().get_camera().get_position().y,
		game.get_renderer().get_camera().get_position().z,
		game.get_renderer().get_camera().get_rotation().x,
		game.get_renderer().get_camera().get_rotation().y,
		game.get_renderer().get_camera().get_camera_distance()
	);
}

void lycoris::profiler::debug_overlay::on_draw()
{
	if (!shown_) return;
	auto& game = game::get_game();
	game.get_renderer().draw_text(text_, format_, color_, canvas_);
}

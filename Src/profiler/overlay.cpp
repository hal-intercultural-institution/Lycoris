#include "profiler/overlay.h"

#include <format>

#include "game.h"

void lycoris::profiler::debug_overlay::on_tick()
{
	if (game::get_game().get_input_system().get_keyboard().is_triggered(VK_F3))
		shown_ = !shown_;
	
}

void lycoris::profiler::debug_overlay::on_draw()
{
	if (!shown_) return;
	auto& game = game::get_game();
	const auto text = std::format(
		L"Lycoris\nfps: {}\nTick: {:f}\nDraw: {:f}\nMouse x: {}\nMouse y: {}",
		game.get_fps_last_second(),
		game.get_last_tick_time(),
		game.get_last_draw_time(),
		game.get_input_system().get_mouse().get_screen_x(),
		game.get_input_system().get_mouse().get_screen_y()
	);
	game.get_renderer().draw_text(text);
}

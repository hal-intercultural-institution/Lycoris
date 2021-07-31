#include "profiler/overlay.h"

#include <format>

#include "game.h"
#include "gamedef.h"

void lycoris::profiler::debug_overlay::on_tick()
{
	
}

void lycoris::profiler::debug_overlay::on_draw()
{
	auto& game = game::get_game();
	const auto text = std::format(
		L"Lycoris\nfps: {}\nTick: {:f}\nDraw: {:f}",
		game.get_fps_last_second(),
		game.get_last_tick_time(),
		game.get_last_draw_time()
	);
	game.get_renderer().draw_text(text);
}

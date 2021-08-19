#pragma once

namespace lycoris::profiler
{
	class debug_overlay
	{
	public:
		void on_tick();
		void on_draw();
	private:
		bool shown_ = false;
	};
}

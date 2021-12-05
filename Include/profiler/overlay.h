#pragma once

#include <string>

#include "render/text.h"

namespace lycoris::profiler
{
	class debug_overlay
	{
	public:
		void initialize();
		void on_tick();
		void on_draw();

	private:
		bool shown_ = false;
		std::wstring text_;
		render::text_canvas canvas_;
		render::text_color color_;
		render::text_format format_;
	};
}

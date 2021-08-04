#pragma once

namespace lycoris::game
{
	class scene
	{
	public:
		virtual ~scene() = default;
		virtual void on_initialize() = 0;
		virtual void on_tick() = 0;
		virtual void on_draw() = 0;
		virtual void on_destroy() = 0;
	};

}
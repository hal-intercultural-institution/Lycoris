#pragma once

namespace lycoris::game
{
	class scene
	{
	public:
		scene() = default;
		scene(const scene&) = delete;
		scene(scene&&) = default;
		virtual ~scene() = default;

		scene& operator=(const scene&) = delete;
		scene& operator=(scene&&) = default;

		// invoked when the library tries to initialize scene
		virtual void on_initialize() = 0;
		// invoked on every tick
		virtual void on_tick() = 0;
		// invoked on every frame, each camera
		virtual void on_draw() = 0;
		// invoked on every frame ONCE regardless how many cameras are enabled
		virtual void on_overlay(){}
		// invoked when the library destroys scene
		virtual void on_destroy() = 0;
	};

}
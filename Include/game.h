#pragma once

#include <memory>

#include "gamedef.h"
#include "render/texture.h"
#include "system/settings.h"
#include "system/input.h"
#include "profiler/logger.h"
#include "profiler/overlay.h"
#include "render/renderer.h"

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

	class game
	{
	public:
		
		std::uint64_t get_frame_count() const
		{
			return frame_count_;
		}

		std::uint32_t get_fps_last_second() const
		{
			return frames_last_second_;
		}
		
		double get_last_tick_time() const
		{
			return frame_time_tick_;
		}
		
		double get_last_draw_time() const
		{
			return frame_time_draw_;
		}
		
		scene& get_current_scene() const;
		render::renderer& get_renderer() noexcept;
		render::texture::texture_loader& get_texture_loader() noexcept;
		system::input::input& get_input_system() noexcept;
		void initialize(HINSTANCE h_instance, int n_show_cmd, MSG* message);
		void run();
		void destroy();

		void set_settings(system::settings& settings);
		void set_scene(std::unique_ptr<scene>&& scene);

		game() = default;
		game(const game&) = delete;
		game& operator=(const game&) = delete;

		profiler::logger logger;

	private:
		std::uint64_t frame_count_ = 0;
		std::unique_ptr<scene> scene_ = nullptr;
		render::renderer renderer_;
		render::texture::texture_loader texture_loader_;
		system::input::input input_system_;
		system::settings settings_;

		profiler::debug_overlay overlay_;

		HINSTANCE h_instance_{};
		MSG* message_ = nullptr;
		
		void on_tick();
		void on_draw();
		
		static void set_game(game* ptr);

		LARGE_INTEGER per_second_{};
		std::uint32_t frames_last_second_ = 0;
		double frame_time_tick_ = 0.0, frame_time_draw_ = 0.0;
	};
	
}

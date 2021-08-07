#pragma once

namespace lycoris::game
{
	class load_screen
	{
	public:
		enum class status
		{
			transition_in,
			loading,
			transition_out,
			not_loading
		};

		// load assets here
		virtual void on_initialize() = 0;
		// this method called when load screen is brought to top, so re-set values here
		virtual void on_reset() = 0;
		virtual void on_tick() = 0;
		virtual void on_draw() = 0;
		virtual void on_destroy() = 0;

		void set_load_status(status status);

		status get_load_status() const;

		virtual ~load_screen() = default;

	protected:
		status status_ = status::not_loading;

	};
}

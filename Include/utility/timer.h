#pragma once

#include <profileapi.h>

namespace lycoris::utility
{
	class timer
	{
	public:
		void start();

		double stop();

		static LARGE_INTEGER frequency;
	private:
		LARGE_INTEGER started_at_{};
	};
}
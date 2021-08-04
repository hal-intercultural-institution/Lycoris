#pragma once

#include <Windows.h>

namespace lycoris::utility
{
	class timer
	{
	public:
		void start();

		double stop();

		inline static LARGE_INTEGER frequency;
	private:
		LARGE_INTEGER started_at_{};
	};
}
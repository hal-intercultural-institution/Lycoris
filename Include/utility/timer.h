#pragma once

#include <profileapi.h>

namespace lycoris::utility
{
	class timer
	{
	public:
		void start()
		{
			QueryPerformanceCounter(&started_at_);
		}

		double stop()
		{
			LARGE_INTEGER stopped_at;
			QueryPerformanceCounter(&stopped_at);
			return static_cast<double>(stopped_at.QuadPart - started_at_.QuadPart) / static_cast<double>(frequency.QuadPart);
		}

		inline static LARGE_INTEGER frequency{};
	private:
		LARGE_INTEGER started_at_{};
	};
}
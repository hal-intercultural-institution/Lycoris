#include "utility/timer.h"

void lycoris::utility::timer::start()
{
	QueryPerformanceCounter(&started_at_);
}

double lycoris::utility::timer::stop()
{
	LARGE_INTEGER stopped_at;
	QueryPerformanceCounter(&stopped_at);
	return static_cast<double>(stopped_at.QuadPart - started_at_.QuadPart) / static_cast<double>(frequency.QuadPart);
}
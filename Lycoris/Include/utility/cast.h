#pragma once
#include <cstdint>

namespace lycoris::utility::scast
{
	inline std::uint64_t uint64_of(const std::uint32_t value)
	{
		return static_cast<std::uint64_t>(value);
	}
}

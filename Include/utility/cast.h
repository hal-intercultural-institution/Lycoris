#pragma once
#include <cstdint>

namespace lycoris::utility::scast
{
	inline std::uint64_t uint64_of(const std::uint32_t value)
	{
		return static_cast<std::uint64_t>(value);
	}

	inline std::uint32_t uint32_of(const std::uint64_t value)
	{
		return static_cast<std::uint32_t>(value);
	}
}

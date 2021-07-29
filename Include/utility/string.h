#pragma once

#include <vector>

namespace lycoris::utility::string
{
	inline std::vector<std::string> split(const std::string& str, const std::string& separator)
	{
		std::vector<std::string> result;
		const std::string work = str + separator;
		const std::uint64_t length = separator.length();
		std::uint64_t pos = 0;
		while (true)
		{
			const std::uint64_t found = work.find(separator, pos);
			if (found == std::string::npos) break;
			result.push_back(work.substr(pos, found - pos));
			pos = found + length;
		}
		return result;
	}
}

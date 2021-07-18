#pragma once

#include <filesystem>
#include <vector>

namespace lycoris::utility::file
{
	std::vector<std::string> load_text_file(const std::filesystem::path& path);
}

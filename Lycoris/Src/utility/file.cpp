#include "utility/file.h"

#include <fstream>
#include <stdexcept>
#include <string>

std::vector<std::string> lycoris::utility::file::load_text_file(const std::filesystem::path& path)
{
	std::ifstream ifs(path);
	if (!ifs.is_open())
	{
		throw std::runtime_error("cannot open file: " + path.string());
	}
	std::string line;
	std::vector<std::string> lines;
	while (std::getline(ifs, line))
	{
		lines.emplace_back(line);
	}
	return lines;
}

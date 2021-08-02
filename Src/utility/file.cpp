#include "utility/file.h"

#include <fstream>
#include <stdexcept>
#include <string>

#include "utility/string.h"

bool lycoris::utility::file::text_file::next_line(std::string& line)
{
	if (!stream_) throw std::runtime_error("TextFile: file is not loaded");
	return !std::getline(stream_, line).fail();
}

bool lycoris::utility::file::text_file::next_splitted_line(std::vector<std::string>& line)
{
	if (!stream_) throw std::runtime_error("TextFile: file is not loaded");
	std::string l;
	if (std::getline(stream_, l).fail()) return false;
	line = string::split(l, " ");
	return true;
}

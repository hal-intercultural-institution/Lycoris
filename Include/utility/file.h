#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace lycoris::utility::file
{
	class text_file
	{
	public:
		text_file(const std::filesystem::path& path)
		{
			path_ = path;
			stream_ = std::ifstream(path);
		}
		text_file(text_file&&) = default;
		text_file(const text_file&) = delete;
		~text_file() = default;
		text_file& operator=(const text_file&) = delete;
		text_file& operator=(text_file&&) = default;
		bool next_line(std::string& line);
		bool next_splitted_line(std::vector<std::string>& line);
	
	private:
		std::ifstream stream_;
		std::filesystem::path path_;
	};
	
}

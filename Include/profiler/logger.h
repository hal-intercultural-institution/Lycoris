#pragma once

#include <filesystem>
#include <fstream>

namespace lycoris::profiler
{
	class logger
	{
	public:
		logger() = default;
		
		logger(const std::filesystem::path& path);

		logger(logger&& other) noexcept;

		logger(const logger&) = delete;

		void log(std::string_view text);

		~logger();

		logger& operator=(logger&& other) noexcept;
		logger& operator=(const logger&) = delete;

	private:
		std::ofstream file_;
	};

}

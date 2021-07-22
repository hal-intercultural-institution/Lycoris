#pragma once

#include <filesystem>
#include <fstream>
#include <chrono>
#include <ctime>

namespace lycoris::profiler
{
	class logger
	{
	public:
		logger();
		
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

	inline logger::logger() = default;

	inline logger::logger(const std::filesystem::path& path)
	{
		file_ = std::ofstream(path, std::ios::app);
	}

	inline logger::logger(logger&& other) noexcept
	{
		file_ = std::move(other.file_);
	}

	inline void logger::log(std::string_view text)
	{
		if (!file_) return;
		const auto timePoint = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(timePoint);
		char buffer[32];
		ctime_s(buffer, sizeof(buffer), &time);
		buffer[std::strlen(buffer) - 1] = ' ';
		file_ << buffer << text << std::endl << std::flush;
	}

	inline logger::~logger()
	{
		file_.close();
	}

	inline logger& logger::operator=(logger&& other) noexcept
	{
		file_ = std::move(other.file_);
		return *this;
	}
}

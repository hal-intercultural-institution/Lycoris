#include "profiler/logger.h"

lycoris::profiler::logger::logger(const std::filesystem::path& path)
{
	file_ = std::ofstream(path, std::ios::app);
}

lycoris::profiler::logger::logger(logger&& other) noexcept
{
	file_ = std::move(other.file_);
}

void lycoris::profiler::logger::log(std::string_view text)
{
	if (!file_) return;
	const auto time_point = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(time_point);
	char buffer[32];
	ctime_s(buffer, sizeof(buffer), &time);
	buffer[std::strlen(buffer) - 1] = ' ';
	file_ << buffer << text << std::endl << std::flush;
}

lycoris::profiler::logger::~logger()
{
	file_.close();
}

lycoris::profiler::logger& lycoris::profiler::logger::operator=(logger&& other) noexcept
{
	file_ = std::move(other.file_);
	return *this;
}

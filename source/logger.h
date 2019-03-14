#pragma once

class tLogger final
{
public:
	virtual ~tLogger() = default;

protected:
	std::shared_ptr<spdlog::logger>	fileLogger		= nullptr;
	std::unique_ptr<spdlog::logger>	consoleLogger	= nullptr;

	std::unique_ptr<spdlog::sinks::basic_file_sink_mt> fileSink;

private:
	tLogger(const std::string& fileName);
};

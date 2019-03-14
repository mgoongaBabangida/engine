#include "stdafx.h"
#include "logger.h"

tLogger::tLogger(const std::string& fileName)
{
	std::ofstream outFile;

	outFile.open(fileName, std::ios::out);
	outFile.close();

	fileSink = std::make_unique<spdlog::sinks::basic_file_sink_mt>("file_sink", fileName);

	spdlog::set_pattern("%^[%T] %n: %v%$");
	fileLogger = spdlog::sinks::basic_file_sink_mt("file_logger", fileSink);
	fileLogger->set_level(spdlog::level::trace);

	consoleLogger = std::make_unique<spdlog::logger>("console_logger");
	consoleLogger->set_level(spdlog::level::trace);
}
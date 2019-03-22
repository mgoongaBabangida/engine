#include "stdafx.h"
#include "logger.h"

std::shared_ptr<spdlog::logger> tLogger::consoleLogger;

tLogger::tLogger(const std::string& name)
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	consoleLogger = spdlog::stdout_color_mt(name.c_str());
	consoleLogger->set_level(spdlog::level::trace);
}
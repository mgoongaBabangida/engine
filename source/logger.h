#pragma once

class tLogger final
{
public:
	virtual ~tLogger() = default;

protected:
	static std::shared_ptr<spdlog::logger> consoleLogger;
private:
	tLogger(const std::string& name);
};

#include "Logger.h"

Logger* Logger::instance = nullptr;

Logger* Logger::GetInstance()
{
	if (!instance)
		instance = new Logger{};
	return instance;
}

void Logger::Drop()
{
	delete instance;
	instance = nullptr;
}

Logger::Logger()
{
#ifdef ENABLE_LOG
		fs.open("log.txt", std::ios::trunc | std::ios::out);
#endif
}

void Logger::Log(std::string str)
{
#ifdef ENABLE_LOG
	fs << str;
#else
	(void)str;// remove warning about unused param
#endif
}

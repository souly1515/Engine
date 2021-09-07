#pragma once
#include <fstream>
#include <string>

#define ENABLE_LOG

class Logger
{
	std::fstream fs;
	static Logger* instance;
public:
	static Logger* GetInstance();
	static void Drop();
	Logger();
	void Log(std::string str);
	static constexpr bool LogEnabled()
	{
#ifdef ENABLE_LOG
		return true;
#else
		return false;
#endif
	}

};


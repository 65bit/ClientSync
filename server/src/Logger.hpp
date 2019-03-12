#pragma once

#include <iostream>
#include <string>

class Logger
{
public:
	enum class Level
	{
		Info,
		Warning,
		Error
	};

private:
	Logger()
	{

	}

	Logger(const Logger&) = delete;
	Logger& operator = (const Logger&) = delete;

public:
	static Logger& Instance()
	{
		static Logger instance;
		return instance;
	}

public:
	void log(const std::string& _message, Level _level = Level::Info)
	{
		std::string prefix;

		switch (_level)
		{
		case Level::Info:
			prefix = "[INFO]";
			break;

		case Level::Warning:
			prefix = "[WARNING]";
			break;

		case Level::Error:
			prefix = "[ERROR]";
			break;

		default:
			prefix = "[UNDEFINED]";
			break;
		}

		std::cout << prefix << _message << std::endl;
	}
};

#define LOG_INFO(__MESSAGE__) do{ std::string msg(__MESSAGE__); Logger::Instance().log(msg + " file:" + __FILE__ + " line:" + std::to_string(__LINE__), Logger::Level::Info); }while(0)
#define LOG_WARNING(__MESSAGE__) do{ std::string msg(__MESSAGE__); Logger::Instance().log(msg + " file:" +  __FILE__ + " line:" +  std::to_string(__LINE__), Logger::Level::Warning); }while(0)
#define LOG_ERROR(__MESSAGE__) do{ std::string msg(__MESSAGE__); Logger::Instance().log(msg + " file:" +  __FILE__ + " line:" +  std::to_string(__LINE__), Logger::Level::Error); }while(0)
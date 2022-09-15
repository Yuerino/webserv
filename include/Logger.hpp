#pragma once

#include <iostream>

#include "LogData.hpp"

#define LOG_I() LOG(webserv::LOG_INFO)
#define LOG_E() LOG(webserv::LOG_ERROR)
#define LOG_D() LOG(webserv::LOG_DEBUG)

#define LOG(log_level) webserv::Logger::getInstance() += webserv::LogData(log_level, __FILE__, __LINE__)

namespace webserv {
	/**
	 * @brief Class Logger is a singleton
	 */
	class Logger {
	public:
		static Logger& getInstance();

		void log(const LogData& log_data);
		void operator+=(const LogData& log_data);

	private:
		Logger();
		~Logger();

		Logger(const Logger& copy); /* disabled */
		Logger& operator=(const Logger& other); /* disabled */
	};
} /* namespace webserv */

#pragma once

#include <iostream>
#include <sstream>

#include "utils.hpp"

namespace webserv {
	enum LogLevel {
		LOG_INFO,
		LOG_ERROR,
		LOG_DEBUG
	};

	static const char* const LogLevelString[] = {
		"INFO",
		"ERROR",
		"DEBUG"
	};

	static const char* const LogLevelColor[] = {
		GREEN,
		RED,
		YELLOW
	};

	class LogData {
	public:
		LogData(enum LogLevel log_level, const char* file, size_t line);
		~LogData();

		template <typename T>
		LogData& operator<<(const T& data) {
			_message << data;
			return *this;
		}

		/* Getters */
		const enum LogLevel& get_log_level() const;
		std::string get_message() const;

	private:
		enum LogLevel _log_level;
		const char* _file;
		size_t _line;
		std::ostringstream _message;

		void set_log_info();

		LogData(const LogData& copy); /* disabled */
		LogData& operator=(const LogData& other); /* disabled */
	};
} /* namespace webserv */
#include "LogData.hpp"

namespace webserv {
	LogData::LogData(LogLevel log_level, const char* file, size_t line) :
		_log_level(log_level), _file(file), _line(line), _message() {
		set_log_info();
	}

	LogData::~LogData() {}

	/**
	 * @brief Set log metadata and add them to message
	 */
	void LogData::set_log_info() {
		_message << BLUE << "[" << get_current_time("%H:%M:%S") << "]" << RESET;
		_message << LogLevelColor[_log_level];
		_message << "[" << LogLevelString[_log_level] << "] ";

#ifdef DEBUG
		if (_log_level == LOG_ERROR || _log_level == LOG_DEBUG)
#else
		if (_log_level == LOG_DEBUG)
#endif
			_message << "[" << _file << ":" << _line << "] ";
	}

	/* Getters */

	const LogLevel& LogData::get_log_level() const { return _log_level; }
	std::string LogData::get_message() const { return _message.str(); }
} /* namespace webserv */

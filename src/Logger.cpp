#include "Logger.hpp"

namespace webserv {
	namespace internal {
		/* Class Logger */

		Logger::Logger() {}

		Logger::~Logger() {}

		/**
		 * @brief Print the log message to correct stream
		 */
		void Logger::log(const LogData& log_data) {
			std::ostream* os;

			switch(log_data.get_log_level()) {
				case LOG_INFO:
					os = &std::cout;
					break;
				case LOG_ERROR:
					os = &std::cerr;
					break;
				case LOG_DEBUG:
					os = &std::clog;
					break;
				default:
					os = &std::cout;
					break;
			}

			*os << log_data.get_message() << RESET << std::flush;
		}

		/**
		 * @note: wrapper for log function because operator+= has higher predence
		 * than operator<< so we can use it in macro function
		 */
		void Logger::operator+=(const LogData& log_data) {
			log(log_data);
		}

		/**
		 * @brief Get the logger singleton instance
		 */
		Logger& Logger::get_instance() {
			static Logger logger;

			return logger;
		}

		/* Class LogData */

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
	} /* namespace internal */
} /* namespace webserv */

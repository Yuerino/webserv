#include "Logger.hpp"

namespace webserv {
	namespace internal {
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
			_message << "[" << get_current_time("%H:%M:%S") << "]";
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

		/* Class Logger */

		Logger::Logger() : _log_fs() {}

		Logger::~Logger() {
			if (_log_fs.is_open()) {
				_log_fs.close();
			}
		}

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

			if (_log_fs.is_open()) {
				_log_fs << log_data.get_message();
				_log_fs.flush();
			}
			*os << BLUE << log_data.get_message().insert(log_data.get_message().find("]") + 1, LogLevelColor[log_data.get_log_level()]) << RESET << std::flush;

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

		void Logger::set_log_file(const std::string& path) {
			if (Logger::get_instance()._log_fs.is_open()) {
				Logger::get_instance()._log_fs.close();
			}

			Logger::get_instance()._log_fs.open(path.c_str(), std::ios::out | std::ios::trunc);

			if (!Logger::get_instance()._log_fs.is_open()) {
				LOG_E() << "Fatal error: failed to open log file: " << path << "\n";
			} else {
				LOG_I() << "Logging to: " << path << "\n";
			}
		}
	} /* namespace internal */
} /* namespace webserv */

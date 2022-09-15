#include "Logger.hpp"

namespace webserv {
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

	Logger& Logger::get_instance() {
		static Logger logger;

		return logger;
	}
} /* namespace webserv */

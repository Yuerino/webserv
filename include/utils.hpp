#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <ctime>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <cerrno>
#include <cstring>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define PURPLE "\033[35m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

#include "Logger.hpp"

#define CRLF "\r\n"

#define LOG_I() LOG(webserv::LOG_INFO)
#define LOG_E() LOG(webserv::LOG_ERROR)
#define LOG_D() LOG(webserv::LOG_DEBUG)

#define LOG(log_level) webserv::internal::Logger::get_instance() += webserv::internal::LogData(log_level, __FILE__, __LINE__)

#define LOG_FILE(path) webserv::internal::Logger::set_log_file(path)

namespace webserv {
	enum requests
	{
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		CONNECT,
		OPTIONS,
		TRACE,
		UNKNOWN
	};

	static const char* const HTTPMethodStrings[] = {
		"GET",
		"HEAD",
		"POST",
		"PUT",
		"DELETE",
		"CONNECT",
		"OPTIONS",
		"TRACE"
	};

	/**
	 * @brief Convert T type to string
	 * @note Only if T type has operator<< overloaded
	 */
	template <typename T>
	std::string to_string(const T& t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	std::string file_to_string(const std::string& file_path);

	std::string get_current_time(const char* format);

	bool is_extension(const std::string& file, const std::string& extension);

	bool is_digits(const std::string& str);

	bool is_ip4(const std::string& ip4);

	bool is_match(std::string str, std::string pattern, char delimiter);

	std::string esc_to_string(std::string const &other);

} /* namespace webserv */

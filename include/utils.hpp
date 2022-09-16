#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <ctime>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define PURPLE "\033[35m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

namespace webserv {
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

	bool is_valid_extension(const std::string& file, const std::string& extension);
} /* namespace webserv */

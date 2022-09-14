#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

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
} /* namespace webserv */

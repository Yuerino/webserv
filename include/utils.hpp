#pragma once

#include <string>
#include <sstream>

namespace webserv {
	template <typename T>
	std::string to_string(const T& t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
} /* namespace webserv */

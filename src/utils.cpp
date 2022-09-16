#include "utils.hpp"
#include "Logger.hpp"

namespace webserv {
	/**
	 * @brief Read from a file and return the content string
	 * @exception Throw runtime_error if file can't be opened
	 */
	std::string file_to_string(const std::string& file_path) {
		std::string str;

		std::ifstream infile(file_path.c_str(), std::ios::in | std::ios::ate);
		if (!infile.is_open()) {
			throw std::runtime_error(std::string("File " + file_path + " can't be opened").c_str());
		}

		str.resize(infile.tellg());
		infile.seekg(0, std::ios::beg);
		infile.read(&str[0], str.size());
		infile.close();

		return str;
	}

	/**
	 * @brief Get the current time in format and return as string
	 */
	std::string get_current_time(const char* format) {
		std::time_t time_epoch = std::time(0);
		char buffer[69];

		std::strftime(buffer, sizeof(buffer), format, std::localtime(&time_epoch));

		return std::string(buffer);
	}

	/**
	 * @brief Check if file has correct extension
	 */
	bool is_valid_extension(const std::string& file, const std::string& extension) {
		size_t ext_pos = file.rfind('.');

		if (ext_pos == std::string::npos || file.substr(ext_pos) != extension)
			return false;

		return true;
	}
} /* namespace webserv */

#include "utils.hpp"

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
} /* namespace webserv */

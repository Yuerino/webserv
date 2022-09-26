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
			throw std::runtime_error("File " + file_path + " can't be opened");
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
	bool is_extension(const std::string& file, const std::string& extension) {
		size_t ext_pos = file.rfind('.');

		if (ext_pos == std::string::npos || file.substr(ext_pos) != extension)
			return false;

		return true;
	}

	/**
	 * @brief Check if string only contains number
	 * @note Doesn't check for decimal number
	 */
	bool is_digits(const std::string& str) {
		std::string str_to_check = str;

		if (str.size() != 1 && str[0] == '-') {
			str_to_check = str.substr(1);
		}

		return str_to_check.find_first_not_of("0123456789") == std::string::npos;
	}

	/**
	 * @brief Check if ip4 string is valid
	 * @note In case of fatal error which should never happen, program will exit
	 */
	bool is_ip4(const std::string& ip4) {
		int r;
		unsigned char buffer[sizeof(in_addr)];

		r = inet_pton(AF_INET, ip4.c_str(), buffer);
		if (r == 0) {
			return false;
		} else if (r < 0) {
			LOG_E() << "Fatal error: inet_pton: " << std::strerror(errno) << "\n";
			std::exit(EXIT_FAILURE);
		}

		return true;
	}

	/**
	 * @brief Check string for wildcard pattern with delimiter to reset matching
	 * @example In case of checking for path, delimiter will be '/'
	 * @note Leetcode problem 44. if you're interested
	 */
	bool is_match(std::string str, std::string pattern, char delimiter) {
		size_t str_len = str.size();
		size_t pattern_len = pattern.size();
		size_t str_i = 0;
		size_t pattern_i = 0;
		size_t backtrack_str_i = 0;
		size_t backtrack_pattern_i = pattern_len;

		while (str_i < str_len) {
			/* advance both index, pattern index will never advance pass end */
			if (str[str_i] == pattern[pattern_i]) {
				/* if found delimiter then reset wildcard matching */
				if (pattern[pattern_i] == delimiter) {
					backtrack_pattern_i = pattern_len;
				}
				++str_i;
				++pattern_i;
				continue;
			}

			/* found '*', track index '*' and current index in str for possible backtrack later*/
			if (pattern[pattern_i] == '*') {
				backtrack_pattern_i = pattern_i;
				backtrack_str_i = str_i;
				++pattern_i;
				continue;
			}

			/* current char doesn't match, last pattern char was '*' but current pattern char isn't '*'
			 * then we start backtracking */
			if (backtrack_pattern_i < pattern_len) {
				pattern_i = backtrack_pattern_i + 1;
				str_i = ++backtrack_str_i;
				continue;
			}

			return false;
		}

		/* check for remaining '*' in pattern */
		while (pattern[pattern_i] == '*') {
			++pattern_i;
		}

		return pattern_i == pattern_len;
	}
} /* namespace webserv */

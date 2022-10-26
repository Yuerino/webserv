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

	/**
	 * @brief Check if path is a file or not
	 */
	bool isPathFile(const std::string& str) {
		std::string path(rtrim(str, "/"));

		struct stat path_info;
		if (stat(path.c_str(), &path_info) == -1) {
			return false;
		}

		if (S_ISREG(path_info.st_mode)) {
			return true;
		} else if (S_ISDIR(path_info.st_mode)) {
			return false;
		}

		return false;
	}

	/**
	 * @brief Trim string from right side
	 */
	std::string rtrim(const std::string &s, const std::string& delimiter) {
		size_t end = s.find_last_not_of(delimiter);
		return (end == std::string::npos) ? s : s.substr(0, end + 1);
	}

	/**
	 * @brief Parse header fields
	 * @throw std::logic_error if header is invalid
	 */
	std::map<std::string, std::string> parse_header_fields(const std::string& raw) {
		std::string headers = raw;
		std::map<std::string, std::string> header_list;
		size_t pos = headers.find("\r\n");

		for (; !headers.empty(); pos = headers.find("\r\n")) {
			std::string possible_header = headers.substr(0, pos);
			if (pos == std::string::npos) {
				headers.clear();
			} else {
				headers = headers.substr(pos + 2);
			}

			size_t header_pos = possible_header.find(": ");
			if (header_pos == std::string::npos) {
				throw new std::logic_error("Invalid header field");
			}

			std::string key = possible_header.substr(0, header_pos);
			std::string value = possible_header.substr(header_pos + 2);

			if (header_list.count(key) > 0) {
				throw new std::logic_error("Duplicate header field");
			}
			header_list[key] = value;
		}

		return header_list;
	}

	/**
	 * @brief Write to a file
	 * @exception Throw runtime_error if file can't be opened
	 */
	void string_to_file(const std::string& file_path, const std::string& content, std::ios::openmode mode) {
		std::ofstream outfile(file_path.c_str(), mode | std::ios::out);
		if (!outfile.is_open()) {
			throw std::runtime_error("File " + file_path + " can't be opened");
		}

		outfile << content;
		outfile.close();
	}

	/**
	 * @brief Get the HTTP Message of status code
	 */
	std::string get_status_message(const int& status_code) {
		switch(status_code) {
			case 200:
				return "OK";
			case 201:
				return "Created";
			case 202:
				return "Accepted";
			case 204:
				return "No Content";
			case 300:
				return "Multiple Choices";
			case 301:
				return "Moved Permanently";
			case 302:
				return "Found";
			case 303:
				return "See Other";
			case 400:
				return "Bad Request";
			case 401:
				return "Unauthorized";
			case 403:
				return "Forbidden";
			case 404:
				return "Not Found";
			case 405:
				return "Method Not Allowed";
			case 413:
				return "Request Too Large";
			case 418:
				return "I'm a teapot";
			case 500:
				return "Internal Server Error";
			case 501:
				return "Not Implemented";
			case 502:
				return "Bad Gateway";
			case 503:
				return "Service Unavailable";
			case 505:
				return "HTTP Version Not Supported";
			default:
				return "Not Implemented";
		}
		return "Not Implemented";
	}

	/**
	 * @brief Get correct mime type from extension
	 */
	std::string get_mime_type(const std::string& type) {
		if (type == ".aac")
			return "audio/aac";
		else if (type == ".abw")
			return "application/x-abiword";
		else if (type == ".arc")
			return "application/octet-stream";
		else if (type == ".avi")
			return "video/x-msvideo";
		else if (type == "avif")
			return "image/avif";
		else if (type == ".azw")
			return "application/vnd.amazon.ebook";
		else if (type == ".bin")
			return "application/octet-stream";
		else if (type == ".bmp")
			return "image/bmp";
		else if (type == ".bz")
			return "application/x-bzip";
		else if (type == ".bz2")
			return "application/x-bzip2";
		else if (type == ".cda")
			return "application/x-cdf";
		else if (type == ".csh")
			return "application/x-csh";
		else if (type == ".css")
			return "text/css";
		else if (type == ".csv")
			return "text/csv";
		else if (type == ".doc")
			return "application/msword";
		else if (type == ".epub")
			return "application/epub+zip";
		else if (type == ".gif")
			return "image/gif";
		else if (type == ".htm")
			return "text/html";
		else if (type == ".html")
			return "text/html";
		else if (type == ".ico")
			return "image/x-icon";
		else if (type == ".ics")
			return "text/calendar";
		else if (type == ".jar")
			return "application/java-archive";
		else if (type == ".jpeg")
			return "image/jpeg";
		else if (type == ".jpg")
			return "image/jpeg";
		else if (type == ".js")
			return "application/js";
		else if (type == ".json")
			return "application/json";
		else if (type == ".mid")
			return "audio/midi";
		else if (type == ".midi")
			return "audio/midi";
		else if (type == ".mpeg")
			return "video/mpeg";
		else if (type == ".mpkg")
			return "application/vnd.apple.installer+xml";
		else if (type == ".odp")
			return "application/vnd.oasis.opendocument.presentation";
		else if (type == ".ods")
			return "application/vnd.oasis.opendocument.spreadsheet";
		else if (type == ".odt")
			return "application/vnd.oasis.opendocument.text";
		else if (type == ".oga")
			return "audio/ogg";
		else if (type == ".ogv")
			return "video/ogg";
		else if (type == ".ogx")
			return "application/ogg";
		else if (type == ".pdf")
			return "application/pdf";
		else if (type == ".png")
			return "image/png";
		else if (type == ".ppt")
			return "application/vnd.ms-powerpoint";
		else if (type == ".rar")
			return "application/x-rar-compressed";
		else if (type == ".rtf")
			return "application/rtf";
		else if (type == ".sh")
			return "application/x-sh";
		else if (type == ".svg")
			return "image/svg+xml";
		else if (type == ".swf")
			return "application/x-shockwave-flash";
		else if (type == ".tar")
			return "application/x-tar";
		else if (type == ".tif")
			return "image/tiff";
		else if (type == ".tiff")
			return "image/tiff";
		else if (type == ".ttf")
			return "application/x-font-ttf";
		else if (type == ".vsd")
			return "application/vnd.visio";
		else if (type == ".wav")
			return "audio/x-wav";
		else if (type == ".weba")
			return "audio/webm";
		else if (type == ".webm")
			return "video/webm";
		else if (type == ".webp")
			return "image/webp";
		else if (type == ".woff")
			return "application/x-font-woff";
		else if (type == ".xhtml")
			return "application/xhtml+xml";
		else if (type == ".xls")
			return "application/vnd.ms-excel";
		else if (type == ".xml")
			return "application/xml";
		else if (type == ".xul")
			return "application/vnd.mozilla.xul+xml";
		else if (type == ".zip")
			return "application/zip";
		else if (type == ".3gp")
			return "video/3gpp audio/3gpp";
		else if (type == ".3g2")
			return "video/3gpp2 audio/3gpp2";
		else if (type == ".7z")
			return "application/x-7z-compressed";

		return "text/plain";
	}
} /* namespace webserv */

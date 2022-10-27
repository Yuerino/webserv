#include "Request.hpp"

namespace webserv {
	Request::Request(struct sockaddr_in client_address, Listen const &server_listen) :
		_status_code(0),
		_client(client_address),
		_raw(),
		_raw_header(),
		_raw_body(),
		_method(-1),
		_path(),
		_query(),
		_headers(),
		_bytes_to_read(0),
		_file_names(),
		_server_listen(server_listen),
		_server_name(),
		_server_config() {}

	Request::Request(Request const &other) :
		_status_code(other._status_code),
		_client(other._client),
		_raw(other._raw),
		_raw_header(other._raw_header),
		_raw_body(other._raw_body),
		_method(other._method),
		_path(other._path),
		_query(other._query),
		_headers(other._headers),
		_bytes_to_read(other._bytes_to_read),
		_file_names(other._file_names),
		_server_listen(other._server_listen),
		_server_name(other._server_name),
		_server_config(other._server_config) {}

	Request& Request::operator=(Request const &other) {
		_status_code = other._status_code;
		_client = other._client;
		_raw = other._raw;
		_raw_header = other._raw_header;
		_raw_body = other._raw_body;
		_method = other._method;
		_path = other._path;
		_query = other._query;
		_headers = other._headers;
		_bytes_to_read = other._bytes_to_read;
		_file_names = other._file_names;
		_server_listen = other._server_listen;
		_server_name = other._server_name;
		_server_config = other._server_config;
		return *this;
	}

	Request::~Request() {}

	void Request::init(const char *raw, size_t size, std::vector<ServerConfig> const &server_configs) {
		_raw = raw;

		try {
			// Separate raw to header and body
			size_t pos = _raw.find("\r\n\r\n");
			if (pos == std::string::npos) {
				_status_code = 400;
				return;
			}
			_raw_header = _raw.substr(0, pos);
			_raw_body.append(raw + pos + 4, size - (_raw_header.size() + 4));

			// Get first line of header
			pos = _raw_header.find("\r\n");
			if (pos == std::string::npos) {
				_status_code = 400;
				return;
			}
			std::string first_line = _raw_header.substr(0, pos);

			if (!parse_method(first_line) || !parse_path(first_line) || !parse_header() ||
				!set_server_config(server_configs) || !parse_body()) {
				return;
			}
		} catch (const std::exception &e) {
			_status_code = 500;
			return;
		}
	}

	/**
	 * @brief Parse method from first line of header
	 */
	bool Request::parse_method(std::string const &first_line) {
		std::string maybe_method = first_line.substr(0, first_line.find(" "));

		for (int i = 0; i < 8; i++) {
			if (maybe_method == HTTPMethodStrings[i]) {
				_method = i;
				return true;
			}
		}

		_method = UNKNOWN;
		_status_code = 400;
		return false;
	}

	/**
	 * @brief Parse path and query from first line of header, also ensure it's HTTP/1.1
	 */
	bool Request::parse_path(std::string const &first_line) {
		std::string maybe_path = first_line.substr(first_line.find(" ") + 1);
		size_t pos = maybe_path.find(" ");
		if (pos == std::string::npos || maybe_path.substr(pos + 1) != "HTTP/1.1") {
			_status_code = 400;
			return false;
		}
		_path = maybe_path.substr(0, pos);

		if (_path.empty()) {
			_status_code = 400;
			return false;
		}

		pos = _path.find("?");
		if (pos != std::string::npos) {
			_query = _path.substr(pos + 1);
			_path = _path.substr(0, pos);
		}

		return true;
	}

	/**
	 * @brief Parse all the header fields
	 */
	bool Request::parse_header() {
		std::string headers = _raw_header.substr(_raw_header.find("\r\n") + 2);
		try {
			_headers = parse_header_fields(headers);
		} catch (const std::logic_error& e) {
			_status_code = 400;
			return false;
		}
		return true;
	}

	/**
	 * @brief Set server config and server name accordingly with host header
	 */
	bool Request::set_server_config(std::vector<ServerConfig> const &server_configs) {
		std::vector<ServerConfig> possible_server_configs;
		std::vector<ServerConfig>::const_iterator it = server_configs.begin();
		for (; it != server_configs.end(); ++it) {
			if (it->get_listens().count(_server_listen) > 0)
				possible_server_configs.push_back(*it);
		}

		if (possible_server_configs.empty()) {
			_status_code = 404;
			return false;
		}

		if (_headers.count("Host") == 0) {
			_status_code = 400;
			return false;
		}

		std::string host_name = _headers.at("Host");
		host_name = host_name.substr(0, host_name.find_first_of(":"));

		std::vector<ServerConfig>::const_iterator s_it = possible_server_configs.begin();
		for (; s_it != possible_server_configs.end(); ++s_it) {
			if (s_it->get_server_names().count(host_name) > 0) {
				_server_name = host_name;
				_server_config = *s_it;
				return true;
			}
		}

		_server_name = *possible_server_configs.at(0).get_server_names().begin();
		_server_config = possible_server_configs.at(0);
		return true;
	}

	/**
	 * @brief Parse body of request
	 */
	bool Request::parse_body() {
		// Set amount of bytes to read if there's "Content-Length"
		if (_bytes_to_read == 0 && _headers.count("Content-Length") > 0) {
			_bytes_to_read = static_cast<size_t>(std::atol(_headers["Content-Length"].c_str()));

			if (_bytes_to_read > (size_t)_server_config.get_client_max_body_size()) {
				_status_code = 413;
				return false;
			}

			if (_bytes_to_read > 0 && !_raw_body.empty()) {
				if (_bytes_to_read < _raw_body.size()) {
					_status_code = 400;
					return false;
				}
				_bytes_to_read -= _raw_body.size();
			}
		}

		return true;
	}

	/**
	 * @brief Append raw buffer to body
	 * @return boolean indicating if body is complete, not for error
	 * @note In case of error, body is completed and status code is set
	 */
	bool Request::append_body(char const *raw, size_t size) {
		if (_bytes_to_read < size) {
			_status_code = 400;
			return true;
		}

		_raw_body.append(raw, size);
		_bytes_to_read -= size;
		return _bytes_to_read == 0;
	}

	/**
	 * @brief Check if request has file upload
	 */
	bool Request::has_files() const {
		return _headers.count("Content-Type") > 0 && _headers.at("Content-Type").find("multipart/form-data") != std::string::npos;
	}

	/**
	 * @brief Write all upload files
	 */
	bool Request::write_files(std::string const &path) {
		if (!has_files()) {
			return false;
		}

		// Get boundary
		std::string content_type = _headers.at("Content-Type");
		if (content_type.find("boundary=") == std::string::npos) {
			_status_code = 400;
			return false;
		}
		std::string boundary = content_type.substr(content_type.find("boundary=") + 9);
		if (boundary.empty()) {
			_status_code = 400;
			return false;
		}

		std::string body = _raw_body;
		size_t pos = body.find("--" + boundary + "\r\n");
		for (; !body.empty() && pos != std::string::npos; pos = body.find("--" + boundary + "\r\n")) {
			std::string chunk = body.substr(pos + boundary.size() + 4);

			// Get chunk header and file data
			size_t file_data_pos = chunk.find("\r\n\r\n");
			if (file_data_pos == std::string::npos) {
				_status_code = 400;
				return false;
			}
			std::string chunk_header = chunk.substr(0, file_data_pos);
			std::string file_data = chunk.substr(file_data_pos + 4);
			size_t file_data_end_pos = file_data.find("\r\n--" + boundary);
			if (file_data_end_pos == std::string::npos) {
				_status_code = 400;
				return false;
			}
			body = file_data.substr(file_data_end_pos + 2);
			file_data = file_data.substr(0, file_data_end_pos);

			// Check if end of boundary and set body
			if (body.substr(0, boundary.size() + 6) == "--" + boundary + "--\r\n") {
				if (body.substr(boundary.size() + 6).empty()) {
					body.clear();
				} else {
					_status_code = 400;
					return false;
				}
			}

			std::map<std::string, std::string> chunk_headers;
			try {
				chunk_headers = parse_header_fields(chunk_header);
			} catch (const std::logic_error &e) {
				_status_code = 400;
				return false;
			}

			// Validate Content-Disposition
			if (chunk_headers.count("Content-Disposition") == 0) {
				_status_code = 400;
				return false;
			}
			std::string content_disposition = chunk_headers.at("Content-Disposition");
			if (content_disposition.find("form-data; name=\"") == std::string::npos) {
				_status_code = 400;
				return false;
			}

			// Get file name, if no file name, skip
			if (content_disposition.find("; filename=\"") == std::string::npos) {
				continue;
			}
			std::string file_name = content_disposition.substr(content_disposition.find("; filename=\"") + 12);
			if (file_name.empty() || file_name.find("\"") == std::string::npos) {
				_status_code = 400;
				return false;
			}
			file_name = file_name.substr(0, file_name.find("\""));
			try {
				string_to_file(path + file_name, file_data);
				_file_names.push_back(file_name);
			} catch (const std::runtime_error &e) {
				_status_code = 500;
				return false;
			}
		}
		return true;
	}

	// Getters
	int const									&Request::get_status_code() const { return (_status_code); }
	struct sockaddr_in const					&Request::get_client() const { return (_client); }
	int const									&Request::get_method() const { return (_method); }
	std::string const							&Request::get_path() const { return (_path); }
	std::string const							&Request::get_query() const { return (_query); }
	std::map<std::string, std::string> const	&Request::get_headers() const { return (_headers); }
	size_t const								&Request::get_bytes_to_read() const { return (_bytes_to_read); }
	std::vector<std::string> const				&Request::get_file_names() const { return (_file_names); }
	Listen const								&Request::get_server_listen() const { return (_server_listen); }
	std::string const							&Request::get_server_name() const { return (_server_name); }
	ServerConfig const							&Request::get_server_config() const { return (_server_config); }
} /* namespace webserv */

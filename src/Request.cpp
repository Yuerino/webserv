#include "Request.hpp"

namespace webserv
{
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
		_upload_file(NULL),
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
		_upload_file(NULL),
		_server_listen(other._server_listen),
		_server_name(other._server_name),
		_server_config(other._server_config) {
		if (other._upload_file)
			_upload_file = new UpFile(*other._upload_file);
	}

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
		if (other._upload_file)
			_upload_file = new UpFile(*other._upload_file);
		_server_listen = other._server_listen;
		_server_name = other._server_name;
		_server_config = other._server_config;
		return *this;
	}

	Request::~Request() {
		if (_upload_file)
			delete (_upload_file);
	}

	void Request::init(std::string const &raw, std::vector<ServerConfig> const &server_configs) {
		_raw = raw;

		// Separate raw to header and body
		size_t pos = _raw.find("\r\n\r\n");
		if (pos == std::string::npos) {
			_status_code = 400;
			return;
		}
		_raw_header = _raw.substr(0, pos);
		_raw_body = _raw.substr(pos + 4);

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
				_status_code = 400;
				return false;
			}

			std::string key = possible_header.substr(0, header_pos);
			std::string value = possible_header.substr(header_pos + 2);

			_headers[key] = value;
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

			if (_bytes_to_read > 0 && _headers.count("Content-Type") == 0) {
				_status_code = 400;
				return false;
			}

			if (_bytes_to_read > 0 && !_raw_body.empty()) {
				set_upload_file(_raw_body.c_str(), _raw_body.size());
				update_bytes_to_read(_raw_body.size());
			}
		}

		return true;
	}

	/**
	 * @brief Update bytes to read from new buffers
	 */
	void Request::update_bytes_to_read(size_t size) {
		if (_bytes_to_read > size)
			_bytes_to_read -= size;
		else
			_bytes_to_read = 0;
	}

	// TODO: refactor upfile
	void Request::set_upload_file(const char *buf, size_t n)
	{
		if (!_upload_file)
			_upload_file = new UpFile;
		_upload_file->append_buf(buf, n);
	}

	int const									&Request::get_status_code(void) const { return (_status_code); }
	struct sockaddr_in const					&Request::get_client(void) const { return (_client); }
	int const									&Request::get_method(void) const { return (_method); }
	std::string const							&Request::get_path(void) const { return (_path); }
	std::string const							&Request::get_query(void) const { return (_query); }
	std::map<std::string, std::string> const	&Request::get_headers(void) const { return (_headers); }
	size_t const								&Request::get_bytes_to_read(void) const { return (_bytes_to_read); }
	UpFile										*Request::get_upload_file(void) const { return (_upload_file); }
	Listen const								&Request::get_server_listen(void) const { return (_server_listen); }
	std::string const							&Request::get_server_name(void) const { return (_server_name); }
	ServerConfig const							&Request::get_server_config(void) const { return (_server_config); }
}

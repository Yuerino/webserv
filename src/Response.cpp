#include "Response.hpp"

namespace webserv {
	Response::Response(Request& request) :
		_request(request),
		_status_code(request.get_status_code()),
		_server_name(request.get_server_name()),
		_server_config(request.get_server_config()),
		_autoindex(false),
		_is_custom_error_page(false),
		_cgi_error(false) {}

	Response::~Response() {}

	/**
	 * @brief Process the request and setup the response accordingly
	 * @note In case of unexpected exception, set status code to 500
	 */
	void Response::process() {
		if (_status_code != 0) {
			return set_error_response();
		}

		try {
			if (!set_location_config() || !set_method()) {
				return set_error_response();
			}

			if (!_location_config.get_redirect().empty()) {
				_status_code = 302;
				_redirect = rtrim(_location_config.get_redirect(), "/") + _target.substr(rtrim(_location_config.get_location(), "/").length());
				return set_redirect_response();
			}

			if (!_cgi_path.empty()) {
				return process_cgi();
			}

			switch (_request.get_method()) {
				case GET:
					return process_get();
				case POST:
					return process_post();
				case DELETE:
					return process_delete();
				default:
					_status_code = 501;
					break;
			}
		} catch (const std::exception& e) {
			_status_code = 500;
		}

		set_error_response();
	}

	/**
	 * @brief Set location config, path, target accordingly with server config and request URI
	 * @return true on success otherwise false and set status code to 404
	 */
	bool Response::set_location_config() {
		std::string location;
		std::string path = _request.get_path();
		if (path.at(path.size() - 1) != '/') {
			path += "/";
		}
		size_t length = path.size();

		for (; length > 0; --length) {
			location = path.substr(0, length);
			if (_server_config.get_locations().count(location)) {
				_location_config = _server_config.get_locations().at(location);
				_target = path;

				if (_location_config.get_root().empty()) {
					_root = _server_config.get_root();
				} else {
					_root = _location_config.get_root();
				}
				return true;
			}
		}

		_status_code = 404;
		return false;
	}

	/**
	 * @brief Check and set method or check and set cgi path
	 * @return true on success otherwise 405 Method not allow or 404 CGI bin not found
	 */
	bool Response::set_method() {
		if (_location_config.get_allow_methods().count(HTTPMethodStrings[_request.get_method()]) == 0) {
			_status_code = 405;
			return false;
		}

		if (!_location_config.get_cgi_path().empty()) {
			_cgi_path = _location_config.get_cgi_path();
			if (access(_cgi_path.c_str(), X_OK) == -1) {
				_status_code = 404;
				return false;
			}

			if (!is_extension(rtrim(_target, "/"), _location_config.get_cgi_extension())) {
				_status_code = 403;
				return false;
			}
		}

		return true;
	}

	/**
	 * @brief Setup CGI data, run CGI and set CGI response
	 */
	void Response::process_cgi() {
		setup_cgi_env();
		std::string cgi_data = run_cgi_script(_cgi_env);

		size_t pos = cgi_data.find("\r\n\r\n");
		if (pos == std::string::npos) {
			_status_code = 500;
			_cgi_error = true;
			return set_error_response();
		}

		try {
			_cgi_headers = parse_header_fields(cgi_data.substr(0, pos));
		} catch (const std::logic_error& e) {
			_status_code = 500;
			_cgi_error = true;
			return set_error_response();
		}

		if (_cgi_headers.count("Location")) {
			_status_code = 302;
		}
		if (_cgi_headers.count("Status")) {
			_status_code = std::atoi(_cgi_headers.at("Status").c_str());
		}
		if (_status_code == 0) {
			_status_code = 200;
		}

		_body = cgi_data.substr(pos + 4);
		set_response();
	}

	/**
	 * @brief Process GET method and setup response
	 */
	void Response::process_get() {
		std::string file_content;

		if (isPathFile(_root + _target)) {
			try {
				_body = file_to_string(rtrim(_root + _target, "/"));
			} catch (const std::exception& e) {
				_status_code = 403;
			}
		} else if (_location_config.get_autoindex() && rtrim(_location_config.get_location(), "/") == rtrim(_target, "/")) {
			_autoindex = true;
			set_autoindex_body();
		} else {
			_target = _root + _target + (_location_config.get_index().empty() ? _server_config.get_index() : _location_config.get_index());

			try {
				_body = file_to_string(_target);
			} catch (const std::exception& e) {
				_status_code = 404;
			}
		}

		if (_status_code >= 400 && _status_code < 600) {
			return set_error_response();
		}

		_status_code = 200;
		set_response();
	}

	void Response::process_post() {
		if (_request.has_files() && !_request.write_files(_root + _target)) {
			_status_code = _request.get_status_code();
			return set_error_response();
		}
		_status_code = 201;
		set_response();
	}

	void Response::process_delete() {
		if (remove(std::string(_root + rtrim(_target, "/")).c_str()) == -1) {
			_status_code = 403;
			return set_error_response();
		}
		_status_code = 204;
		set_response();
	}

	/**
	 * @brief Set response header and body
	 * @note Should only be call after process and set the response body
	 */
	void Response::set_response() {
		_response = "HTTP/1.1 ";
		_response += to_string(_status_code);
		_response += " " + get_status_message(_status_code);
		_response += CRLF;

		_response += "Date: ";
		_response += get_current_time("%a, %d %b %Y %H:%M:%S %Z");
		_response += CRLF;

		_response += "Server: ";
		_response += "webserv/6.9";
		_response += CRLF;

		_response += "Connection: close";
		_response += CRLF;

		if (!_cgi_path.empty() && !_cgi_error) {
			std::map<std::string, std::string>::iterator it = _cgi_headers.begin();
			for (; it != _cgi_headers.end(); ++it) {
				if (it->first == "Status") {
					continue;
				}
				_response += it->first + ": " + it->second + CRLF;
			}
			if (_cgi_headers.count("Content-Length") == 0) {
				_response += "Content-Length: ";
				_response += to_string(_body.size());
				_response += CRLF;
			}

			_response += CRLF;
			_response += _body;
			return;
		}

		_response += "Content-Length: ";
		_response += to_string(_body.size());
		_response += CRLF;

		if (_status_code >= 400 && _status_code < 600) {
			_response += "Content-Type: ";
			if (_is_custom_error_page && rtrim(_target, "/").find_last_of('.') != std::string::npos) {
				_response += get_mime_type(rtrim(_target, "/").substr(rtrim(_target, "/").find_last_of('.')));
			} else {
				_response += "text/html";
			}
			_response += CRLF;

			_response += CRLF;
			_response += _body;
			return;
		}

		if (_request.get_method() == GET) {
			_response += "Content-Type: ";
			if (_autoindex || !_cgi_path.empty()) {
				_response += "text/html";
			} else if (rtrim(_target, "/").find_last_of('.') != std::string::npos) {
				_response += get_mime_type(rtrim(_target, "/").substr(rtrim(_target, "/").find_last_of('.')));
			} else {
				_response += "text/plain";
			}
			_response += CRLF;
		}

		if (_request.get_method() == POST && _request.has_files()) {
			_response += "Location: ";
			std::string filename = _request.get_file_names().at(0);
			_response += _target + filename;
			_response += CRLF;
		}

		get_cookies();

		_response += CRLF;
		_response += _body;
	}

	void Response::get_cookies() {
		if (_request.get_headers().count("Cookie") == 0 || (_request.get_headers().count("Cookie") > 0 && _request.get_headers().at("Cookie").find("timestamp=") == std::string::npos)) {
			_response += "Set-Cookie: ";
			_response += "timestamp=" + get_current_time("%H:%M:%S") + "; Max-Age=30";
			_response += CRLF;
		}
	}

	/**
	 * @brief Set the response body and header for error status code
	 */
	void Response::set_error_response() {
		if (_server_config.get_error_pages().count(to_string(_status_code))) {
			_target = _root + _server_config.get_error_pages().at(to_string(_status_code));
			try {
				_body = file_to_string(_target);
				_is_custom_error_page = true;
				return set_response();
			} catch (const std::exception& e) { (void)e; }
		}

		_body = "<html>\n";

		_body += "<head><title>";
		_body += get_status_message(_status_code);
		_body += "</title></head>\n";

		_body += "<body><center><h1>";
		_body += get_status_message(_status_code);
		_body += "</h1></center></body>\n";

		_body += "</html>";

		set_response();
	}

	/**
	 * @brief Setup autoindex body
	 */
	void Response::set_autoindex_body() {
		std::string path = rtrim(_root + _target, "/");

		struct dirent* file = NULL;
		DIR *dir = opendir(path.c_str());
		if (dir == NULL) {
			_status_code = 500;
			return;
		}

		_body = "<html>\n";
		_body += "<head><title>" + path + "</title></head>\n";
		_body += "<body>\n";
		_body += "<h1>index of " + path + "</h1>\n";

		file = readdir(dir);
		while (file != NULL) {
			std::string file_name(file->d_name);
			_body += "<p><a href=\"http://" + _request.get_headers().at("Host") + rtrim(_target, "/") + "/" + file_name + "\">" + file_name + "</a></p>";
			file = readdir(dir);
		}

		_body += "</body>\n";
		_body += "</html>";
		closedir(dir);
	}

	/**
	 * @brief Setup redirect response header
	 */
	void Response::set_redirect_response() {
		_response = "HTTP/1.1 ";
		_response += to_string(_status_code);
		_response += " " + get_status_message(_status_code);
		_response += CRLF;

		_response += "Date: ";
		_response += get_current_time("%a, %d %b %Y %H:%M:%S %Z");
		_response += CRLF;

		_response += "Server: ";
		_response += "webserv/6.9";
		_response += CRLF;

		_response += "Connection: close";
		_response += CRLF;

		_response += "Location: ";
		_response += _redirect;
		_response += CRLF;

		get_cookies();

		_response += CRLF;
	}

	void Response::setup_cgi_env() {
		_cgi_env["SERVER_SOFTWARE"] = "webserv/6.9";
		_cgi_env["SERVER_NAME"] = _server_name;
		_cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
		_cgi_env["SERVER_PROTOCOL"] = "HTTP/1.1";
		_cgi_env["SERVER_PORT"] = to_string(_request.get_server_listen().port);
		_cgi_env["REQUEST_METHOD"] = HTTPMethodStrings[_request.get_method()];

		_cgi_env["REQUEST_URI"] = _root + rtrim(_target, "/");
		_cgi_env["SCRIPT_NAME"] = rtrim(_target, "/");
		_cgi_env["PATH_INFO"] = _cgi_path;
		_cgi_env["PATH_TRANSLATED"] = _root + rtrim(_target, "/");
		_cgi_env["QUERY_STRING"] = _request.get_query();

		_cgi_env["AUTH_TYPE"] = "";
		_cgi_env["REMOTE_USER"] = "";

		_cgi_env["CONTENT_TYPE"] = "";
		_cgi_env["CONTENT_LENGTH"] = "";
		_cgi_env["REDIRECT_STATUS"] = "1";

		char client_address[69];
		inet_ntop(AF_INET, &(_request.get_client().sin_addr), client_address, 69);
		_cgi_env["REMOTE_ADDR"] = std::string(client_address);

		// Request header HTTP
		std::map<std::string, std::string>::const_iterator header_it = _request.get_headers().begin();
		for (; header_it != _request.get_headers().end(); ++header_it) {
			std::string header_name = header_it->first;
			for (size_t i = 0; i < header_name.length(); ++i) {
				header_name[i] = toupper(header_name[i]);
			}
			_cgi_env["HTTP_" + header_name] = header_it->second;
		}
	}

	/* Getter */
	const std::string& Response::get_raw_data() const { return _response; }
} /* namespace webserv */

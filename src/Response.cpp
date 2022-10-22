#include "Response.hpp"

namespace webserv {
	Response::Response(const std::vector<ServerConfig>& server_configs, const Request& request) :
		_server_configs(),
		_request(request),
		_status_code(request.get_flag()) {
		_autoindex = false;
		_is_custom_error_page = false;

		std::vector<ServerConfig>::const_iterator it = server_configs.begin();
		for (; it != server_configs.end(); ++it) {
			if (it->get_listens().count(request.get_server_listen()) > 0)
				_server_configs.push_back(*it);
		}

		if (_server_configs.empty()) {
			_status_code = 404;
		}
	}

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
			if (!set_server_config() || !set_location_config() || !set_method()) {
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
				// case HEAD:
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
	 * @brief Set server config and server name accordingly with host header
	 * @return true on success otherwise false and set status code to 400
	 */
	bool Response::set_server_config() {
		if (_request.get_content().count("Host") == 0) {
			_status_code = 400;
			return false;
		}

		std::string host_name = _request.get_content().at("Host");
		host_name = host_name.substr(0, host_name.find_first_of(":"));

		std::vector<ServerConfig>::const_iterator s_it = _server_configs.begin();
		std::set<std::string>::const_iterator n_it;
		for (; s_it != _server_configs.end(); ++s_it) {
			if (s_it->get_server_names().count(host_name) > 0) {
				_server_name = host_name;
				_server_config = *s_it;
				return true;
			}
		}

		_server_name = *_server_configs.at(0).get_server_names().begin();
		_server_config = _server_configs.at(0);
		return true;
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

			if (access((/*_root + _location_config.get_location() + */_cgi_path).c_str(), X_OK) == -1) {
				_status_code = 404;
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
		// TODO: parse cgi response
		_body = run_cgi_script(_cgi_env);
		_status_code = 200;
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

		// if (_request.get_method() == HEAD) {
		// 	_body = "";
		// }

		_status_code = 200;
		set_response();
	}

	void Response::process_post() {
		if (!_request.get_UpFile()->is_file())
			return ;
		try
		{
			_request.get_UpFile()->write_to_file(_root + _target);
		}
		catch(const std::exception& e)
		{
			_status_code = 400;
			set_error_response();
		}
		_status_code = 201;
		set_response();
	}

	void Response::process_delete() {
		if (!remove((const char *)std::string(_root + rtrim(_target, "/")).c_str())) {
			_status_code = 204;
			set_response();
		}
		else {
			_status_code = 400;
			set_error_response();
		}
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

		_response += "Content-Length: ";
		_response += to_string(_body.length());
		_response += CRLF;

		if (_request.get_method() == GET) {
			_response += "Content-Type: ";
			if (_autoindex || (_status_code >= 400 && _status_code < 600 && !_is_custom_error_page) || !_cgi_path.empty()) {
				_response += "text/html";
			} else if (rtrim(_target, "/").find_last_of('.') != std::string::npos) {
				_response += get_mime_type(rtrim(_target, "/").substr(rtrim(_target, "/").find_last_of('.')));
			} else {
				_response += "text/plain";
			}
			_response += CRLF;
		}

		if (_request.get_method() == POST && _request.get_UpFile() != NULL) {
			_response += "Location: ";
			std::string filename = _request.get_UpFile()->get_files().begin()->first;
			_response += _target + filename;
			_response += CRLF;
		}

		get_cookies();

		_response += CRLF;
		if (_body.find("<html>") != std::string::npos)
			_response += _body.substr(_body.find("<html>"));
		else
			_response += _body;
	}

	void Response::get_cookies() {
		if (_request.get_content().count("Cookie") == 0 || (_request.get_content().count("Cookie") > 0 && _request.get_content().at("Cookie").find("timestamp=") == std::string::npos)) {
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
			_body += "<p><a href=\"http://" + _request.get_content().at("Host") + rtrim(_target, "/") + "/" + file_name + "\">" + file_name + "</a></p>";
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

	/* Getter */
	const std::string& Response::get_raw_data() const {
		return _response;
	}

	/**
	 * @brief Get the HTTP Message of status code
	 * @note Static function
	 */
	std::string Response::get_status_message(const int& status_code) {
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

	void Response::setup_cgi_env() {
		_cgi_env["SERVER_SOFTWARE"] = "webserv/6.9";
		_cgi_env["SERVER_NAME"] = _server_name;
		_cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
		_cgi_env["SERVER_PROTOCOL"] = "HTTP/1.1";
		_cgi_env["SERVER_PORT"] = to_string(_request.get_server_listen().port);
		_cgi_env["REQUEST_METHOD"] = HTTPMethodStrings[_request.get_method()];

		_cgi_env["REQUEST_URI"] = _root + rtrim(_target, "/");
		_cgi_env["SCRIPT_NAME"] = rtrim(_target, "/").substr(0, _target.find("?"));
		_cgi_env["PATH_INFO"] = _cgi_path;
		_cgi_env["PATH_TRANSLATED"] = _root + rtrim(_target, "/").substr(0, _target.find("?"));
		if (_target.find("?") != std::string::npos)
			_cgi_env["QUERY_STRING"] = rtrim(_target.substr(_target.find("?") + 1), "/"); // TODO
		else
			_cgi_env["QUERY_STRING"] = "";

		_cgi_env["AUTH_TYPE"] = "";
		_cgi_env["REMOTE_USER"] = "";

		_cgi_env["CONTENT_TYPE"] = "";
		_cgi_env["CONTENT_LENGTH"] = "";
		_cgi_env["REDIRECT_STATUS"] = "1";

		char client_address[69];
		inet_ntop(AF_INET, &(_request.get_client().sin_addr), client_address, 69);
		_cgi_env["REMOTE_ADDR"] = std::string(client_address);

		// TODO: add remaining cgi env from header
	}
} /* namespace webserv */

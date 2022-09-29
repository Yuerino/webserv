#include "Response.hpp"

namespace webserv {
	Response::Response(const std::vector<ServerConfig>& server_configs, const Request& request) : _server_configs(), _request(request), _status_code(0) {
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
	 */
	void Response::process() {

		if (_status_code != 0) {
			return set_error_response();
		}

		if (!set_server_config() || !set_location_config() || !set_method()) {
			return set_error_response();
		}

		if (!_cgi_path.empty()) {
			return run_cgi();
		}

		switch (_request.get_method()) {
			case GET:
				return process_get();
			default:
				_status_code = 400;
				break;
		}

		set_error_response();
	}

	/**
	 * @brief Set server config accordingly with host header
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
				_server_config = *s_it;
				return true;
			}
		}

		_server_config = _server_configs.at(0);
		return true;
	}

	/**
	 * @brief Set location config accordingly with server config and request URI
	 * @return true on success otherwise false and set status code to 404
	 */
	bool Response::set_location_config() {
		if (_server_config.get_locations().count(_request.get_path()) == 0) {
			_status_code = 404;
			return false;
		}

		_location_config = _server_config.get_locations().at(_request.get_path());
		return true;
	}

	/**
	 * @brief Check and set method or setup cgi
	 * @return true on success otherwise 405 Method not allow or ??? CGI bin not found
	 */
	bool Response::set_method() {
		return true;
	}

	/**
	 * @brief Setup CGI data, run CGI and set CGI response
	 */
	void Response::run_cgi() {
		_status_code = 501;
		set_error_response();
	}

	/**
	 * @brief Process GET method and setup response
	 */
	void Response::process_get() {
		_status_code = 200;
		_body = file_to_string("./html/post.html");
		// _body = "<html><body><h1>Hello World</h1><body></html>";
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
		_response += "webserv v6.9";
		_response += CRLF;

		_response += "Connection: close";
		_response += CRLF;

		_response += "Content-Length: ";
		_response += to_string(_body.length());
		_response += CRLF;

		// TODO: MIME types
		_response += "Content-Type: ";
		_response += "text/html";
		_response += CRLF;

		_response += CRLF;
		_response += _body;
	}

	/**
	 * @brief Set the response body and header for error status code
	 */
	void Response::set_error_response() {
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
} /* namespace webserv */

#include "Response.hpp"

namespace webserv {
	Response::Response() : _status_code(0) {}

	Response::~Response() {}

	/**
	 * @brief Process the request and setup the response accordingly
	 */
	void Response::process() {
		_status_code = 400;
		set_error_response();
	}

	/* Getter */
	const std::string& Response::get_raw_data() const {
		return _response;
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

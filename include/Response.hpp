#pragma once

#include <string>
#include <map>

#include "utils.hpp"

namespace webserv {
	class Response {
	public:
		Response();
		~Response();

		void process();

		const std::string& get_raw_data() const;
		static std::string get_status_message(const int& status_code);

	private:
		int _status_code;
		std::map<std::string, std::string> _fields;
		std::string _response;
		std::string _body;

		// void process_get();
		// void process_post();
		// void process_delete();

		void set_response();
		void set_error_response();

		Response(const Response& copy); /* disabled */
		Response& operator=(const Response&other); /* disabled */
	};
} /* namespace webserv */

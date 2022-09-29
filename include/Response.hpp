#pragma once

#include <string>
#include <map>

#pragma once

#include "utils.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"

namespace webserv {
	class Response {
	public:
		Response(const std::vector<ServerConfig>& server_configs, const Request& requestz);
		~Response();

		void process();

		const std::string& get_raw_data() const;
		static std::string get_status_message(const int& status_code);

	private:
		std::vector<ServerConfig> _server_configs;
		const Request& _request;
		int _status_code;
		std::map<std::string, std::string> _fields;
		std::string _response;
		std::string _body;
		std::string _path;
		std::string _cgi_path;
		ServerConfig _server_config;
		LocationConfig _location_config;

		bool set_server_config();
		bool set_location_config();
		bool set_method();
		void process_cgi();
		void process_get();
		// void process_post();
		// void process_delete();
		void set_response();
		void set_error_response();

		Response(const Response& copy); /* disabled */
		Response& operator=(const Response&other); /* disabled */
	};
} /* namespace webserv */

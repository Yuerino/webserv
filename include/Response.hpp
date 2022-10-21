#pragma once

#include <string>
#include <map>
#include <dirent.h>

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
		std::string _target;
		std::string _root;
		std::string _cgi_path;
		std::string _server_name;
		ServerConfig _server_config;
		LocationConfig _location_config;
		std::map<std::string, std::string> _cgi_env;
		bool _autoindex;
		std::string _redirect;
		bool _is_custom_error_page;

		bool set_server_config();
		bool set_location_config();
		bool set_method();
		void process_cgi();
		void process_get();
		void process_post();
		void process_delete();
		void set_response();
		void set_error_response();
		void setup_cgi_env();
		void set_autoindex_body();
		void set_redirect_response();
		void get_cookies();

		Response(const Response& copy); /* disabled */
		Response& operator=(const Response&other); /* disabled */
	};

} /* namespace webserv */

std::string run_cgi_script(std::map<std::string, std::string> envp_map);

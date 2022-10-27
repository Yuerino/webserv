#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <vector>
#include <netinet/in.h>
#include <cstdlib>

#include "utils.hpp"
#include "ServerConfig.hpp"

namespace webserv {
	class Request {
		private:
			int									_status_code;
			struct sockaddr_in					_client;
			std::string							_raw;
			std::string							_raw_header;
			std::string							_raw_body;
			int									_method;
			std::string							_path;
			std::string							_query;
			std::map<std::string, std::string>	_headers;
			size_t								_bytes_to_read;
			std::vector<std::string>			_file_names;
			Listen								_server_listen;
			std::string							_server_name;
			ServerConfig						_server_config;

			bool	parse_method(std::string const &first_line);
			bool	parse_path(std::string const &first_line);
			bool	parse_header();
			bool	set_server_config(std::vector<ServerConfig> const &server_configs);
			bool 	parse_body();

		public:
			Request(struct sockaddr_in client_address, Listen const &server_listen);
			Request(Request const &other);
			Request& operator=(Request const &other);
			~Request();

			void										init(const char *raw, size_t size, std::vector<ServerConfig> const &server_config);
			bool										append_body(const char *raw, size_t size);
			void										set_upload_file();
			bool										has_files() const;
			bool										write_files(std::string const &path);
			int const									&get_status_code() const;
			struct sockaddr_in const					&get_client() const;
			int	const 									&get_method() const;
			std::string const							&get_path() const;
			std::string const							&get_query() const;
			std::map<std::string, std::string> const	&get_headers() const;
			size_t const								&get_bytes_to_read() const;
			std::vector<std::string> const				&get_file_names() const;
			Listen const								&get_server_listen() const;
			std::string const							&get_server_name() const;
			ServerConfig const							&get_server_config() const;
	};
} /* namespace webserv */

#endif

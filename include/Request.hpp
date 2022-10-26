#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <vector>
#include <netinet/in.h>
#include <cstdlib>

#include "UpFile.hpp"
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
			UpFile								*_upload_file;
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

			void										init(std::string const &request, std::vector<ServerConfig> const &server_config);
			int const									&get_status_code(void) const;
			struct sockaddr_in const					&get_client(void) const;
			int	const 									&get_method(void) const;
			std::string const							&get_path(void) const;
			std::string const							&get_query(void) const;
			std::map<std::string, std::string> const	&get_headers(void) const;
			size_t const								&get_bytes_to_read(void) const;
			UpFile										*get_upload_file(void) const;
			Listen const								&get_server_listen(void) const;
			std::string const							&get_server_name(void) const;
			ServerConfig const							&get_server_config(void) const;
			void										set_upload_file(const char *buf, size_t n);
			void										update_bytes_to_read(size_t size);
	};
}

#endif

// GET REQUEST EXAMPLE
// GET / HTTP/1.1
// Host: localhost:8000
// Connection: keep-alive
// sec-ch-ua: "Google Chrome";v="105", "Not)A;Brand";v="8", "Chromium";v="105"
// sec-ch-ua-mobile: ?0
// sec-ch-ua-platform: "macOS"
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9,it-IT;q=0.8,it;q=0.7

// POST REQUEST EXAMPLE
// POST /somewhere_else HTTP/1.1
// Host: localhost:8000
// Connection: keep-alive
// Content-Length: 17
// Cache-Control: max-age=0
// sec-ch-ua: "Google Chrome";v="105", "Not)A;Brand";v="8", "Chromium";v="105"
// sec-ch-ua-mobile: ?0
// sec-ch-ua-platform: "macOS"
// Upgrade-Insecure-Requests: 1
// Origin: http://localhost:8000
// Content-Type: application/x-www-form-urlencoded
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
// Sec-Fetch-Site: same-origin
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Referer: http://localhost:8000/
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9,it-IT;q=0.8,it;q=0.7
// Cookie: random-test=ThisIsTheTest
// lname=hello+world

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

namespace webserv
{
	class Request
	{
		private:
			int									_method;
			int									_status_code;
			std::string							_path;
			struct sockaddr_in					_client;
			std::map<std::string, std::string>	_content;
			unsigned long						_bytes_to_read;
			UpFile								*_file_to_upload;
			Listen								_server_listen;
			std::string							_server_name;
			ServerConfig						_server_config;
			std::string							_raw;

		public:
			Request();
			Request(struct sockaddr_in client_address, Listen server_listen);
			Request(Request const &other);
			Request& operator=(Request const &other);
			~Request();

			void										init(std::string const &request, std::vector<ServerConfig> const &server_config);
			int											get_method(void) const;
			int											get_status_code(void) const;
			std::string const							&get_path(void) const;
			struct sockaddr_in const					&get_client(void) const;
			std::map<std::string, std::string> const	&get_content(void) const;
			const unsigned long							&get_bytes_to_read(void) const;
			UpFile										*get_UpFile(void) const;
			Listen const								&get_server_listen(void) const;
			std::string const							&get_server_name(void) const;
			ServerConfig const							&get_server_config(void) const;

			void						parse_method();
			void						parse_path();
			void						assign_content();
			std::string	const			parse_field(std::string const &fieldName);
			void						set_bytes_to_read(void);
			void						mod_bytes_to_read(int mod);
			void						set_UpFile(char *buf, size_t n);
			bool						check_single_chunk(std::string buffer);
			bool						set_server_config();
	};
}

#endif

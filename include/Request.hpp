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

namespace webserv
{
	enum requests
	{
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		CONNECT,
		OPTIONS,
		TRACE,
		UNKNOWN
	};

	static const char* const HTTPMethodStrings[] = {
		"GET",
		"HEAD",
		"POST",
		"PUT",
		"DELETE",
		"CONNECT",
		"OPTIONS",
		"TRACE"
	};

	class Request
	{
		private:
			int									_method;
			std::string	const					_path;
			std::string const					_scheme;
			struct sockaddr_in const			_client;
			std::map<std::string, std::string>	_content;
			unsigned long						_bytes_to_read;
			UpFile								*_file_to_upload;
		
		public:
			Request(std::string const &request, struct sockaddr_in clientAddr);
			Request(Request const &other);
			~Request();

			int											get_method(void) const;
			std::string const							&get_path(void) const;
			std::string const							&get_scheme(void) const;
			struct sockaddr_in const					&get_client(void) const;
			std::map<std::string, std::string> const	&get_content(void) const;
			const unsigned long							&get_bytes_to_read(void) const;
			UpFile										*get_UpFile(void) const;

			int							parse_method(std::string const &src);
			std::string	const			parse_path(std::string const &src);
			std::string	const			parse_field(std::string const &src, std::string const &fieldName);
			void						assign_content(std::string const &src);
			void						set_bytes_to_read(void);
			void						mod_bytes_to_read(int mod);
			void						set_UpFile(std::string buffer);
	};
}

#endif

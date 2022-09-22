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

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <netinet/in.h>

namespace webserv
{
	enum requests
	{
		GET,
		POST,
		DELETE,
		UNKNOWN
	};

	class Request
	{
		private:
			int							_method;
			std::string	const			_path;
			std::string const			_scheme;
			std::string	const			_host;
			struct sockaddr_in const	_client;
		
		public:
			Request(std::string const &request, struct sockaddr_in clientAddr);
			Request(Request const &other);
			~Request();

			int							get_method(void) const;
			std::string const			&get_path(void) const;
			std::string const			&get_scheme(void) const;
			std::string const			&get_host(void) const;
			struct sockaddr_in const	&get_client(void) const;

			int							parse_method(std::string const &src);
			std::string	const			parse_path(std::string const &src);
			std::string	const			parse_field(std::string const &src, std::string const &fieldName);
	};
}

#endif

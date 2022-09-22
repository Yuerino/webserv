#include "Request.hpp"
#include "utils.hpp"

namespace webserv
{
	Request::Request(std::string const &request, struct sockaddr_in clientAddr) :
		_method(webserv::sub_substr(request, '\n', ' ', 0, 0)),
		_path(webserv::sub_substr(request, '\n', ' ', 0, 1)),
		_scheme("http"),
		_host(webserv::sub_substr(request, '\n', ' ', 1, 1)),
		_client(clientAddr)
	{}

	Request::Request(Request const &other) :
		_method(other._method),
		_path(other._path),
		_scheme("http"),
		_host(other._host),
		_client(other._client)
	{}

	Request::~Request()
	{}

	std::string const &Request::get_method(void) const
	{
		return (_method);
	}

	std::string const &Request::get_path(void) const
	{
		return (_path);
	}

	std::string const &Request::get_scheme(void) const
	{
		return (_scheme);
	}

	std::string const &Request::get_host(void) const
	{
		return (_host);
	}

	struct sockaddr_in const &Request::get_client(void) const
	{
		return (_client);
	}
}
#include "Request.hpp"
#include "utils.hpp"

namespace webserv
{
	Request::Request(std::string const &request, struct sockaddr_in clientAddr) :
		_method(parse_method(request)),
		_path(parse_path(request)),
		_scheme("http"),
		_host(parse_field(request, "Host")),
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

	int					Request::get_method(void) const
	{
		return (_method);
	}

	std::string const	&Request::get_path(void) const
	{
		return (_path);
	}

	std::string const	&Request::get_scheme(void) const
	{
		return (_scheme);
	}

	std::string const	&Request::get_host(void) const
	{
		return (_host);
	}

	struct sockaddr_in const &Request::get_client(void) const
	{
		return (_client);
	}

	int					Request::parse_method(std::string const &src)
	{
		try
		{
			std::string res(src.substr(0, src.find(" ")));
			if (res == "GET")
				return GET;
			else if (res == "POST")
				return POST;
			else if (res == "DELETE")
				return DELETE;
			else
				return UNKNOWN;
		}
		catch (const std::exception& e)
		{
			return UNKNOWN;
		}
	}

	std::string	const		Request::parse_path(std::string const &src)
	{
		try
		{
			uint32_t	pos = src.find(" ") + 1;
			return (src.substr(pos, src.find(" ", pos) - pos));
		}
		catch(const std::exception& e)
		{
			return ("");
		}
	}

	std::string	const		Request::parse_field(std::string const &src, std::string const &fieldName)
	{
		try
		{
			uint32_t	pos_field(0);
			pos_field = src.find(fieldName);
			std::string	line(src.substr(pos_field, src.find("\n", pos_field) - pos_field));
			return (line.substr(line.find(" ") + 1));
		}
		catch(const std::exception& e)
		{
			return ("");
		}
	}
}

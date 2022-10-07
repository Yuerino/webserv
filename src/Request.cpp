#include "Request.hpp"

namespace webserv
{
	Request::Request() :
		_method(-1),
		_path(),
		_scheme(),
		_client(),
		_bytes_to_read(0),
		_file_to_upload(NULL),
		_server_listen()
	{}

	Request::Request(struct sockaddr_in client_address, Listen server_listen) :
		_method(-1),
		_path(),
		_scheme(),
		_client(client_address),
		_bytes_to_read(0),
		_file_to_upload(NULL),
		_server_listen(server_listen)
	{}

	Request::Request(Request const &other) :
		_method(other._method),
		_path(other._path),
		_scheme(other._scheme),
		_client(other._client),
		_content(other._content),
		_bytes_to_read(other._bytes_to_read),
		_file_to_upload(NULL),
		_server_listen(other._server_listen)
	{
		if (other._file_to_upload)
			_file_to_upload = new UpFile(*other._file_to_upload);
	}

	Request& Request::operator=(Request const &other)
	{
		_method = other._method;
		_path = other._path;
		_scheme = other._scheme;
		_client = other._client;
		_content = other._content;
		_bytes_to_read = other._bytes_to_read;
		_server_listen = other._server_listen;
		if (other._file_to_upload)
			_file_to_upload = new UpFile(*other._file_to_upload);
		return *this;
	}

	Request::~Request()
	{
		if (_file_to_upload)
			delete (_file_to_upload);
	}

	void	Request::init(std::string const &request)
	{
		_method = parse_method(request);
		_path = parse_path(request);
		_scheme = "http";
		assign_content(request);
	}

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

	struct sockaddr_in const &Request::get_client(void) const
	{
		return (_client);
	}

	unsigned long const	&Request::get_bytes_to_read(void) const
	{
		return (_bytes_to_read);
	}

	std::map<std::string, std::string> const	&Request::get_content(void) const
	{
		return (_content);
	}

	Listen const								&Request::get_server_listen(void) const
	{
		return (_server_listen);
	}

	int					Request::parse_method(std::string const &src)
	{
		try
		{
			std::string res(src.substr(0, src.find(" ")));
			int	i(0);
			while (res != HTTPMethodStrings[i] && i < 8)
				i++;
			return i;
		}
		catch (const std::exception& e)
		{
			return UNKNOWN;
		}
	}

	UpFile				*Request::get_UpFile(void) const
	{
		return (_file_to_upload);
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

	void					Request::assign_content(std::string const &src)
	{
		size_t	pos(0);
		pos = src.find("\r\n", 0);
		while (pos != std::string::npos)
		{
			pos += 2;
			if (src.find(":", pos) == std::string::npos)
				break;
			std::string key = src.substr(pos, src.find(":", pos) - pos);
			_content[key] = parse_field(src, key);
			pos = src.find("\r\n", pos);
		}
	}

	void				Request::set_bytes_to_read(void)
	{
		if (!_bytes_to_read && _content.find("Content-Length") != _content.end())
			_bytes_to_read = std::atol(_content["Content-Length"].c_str());
	}

	void				Request::mod_bytes_to_read(int mod)
	{
		if ((unsigned long)mod > _bytes_to_read)
			_bytes_to_read = 0;
		else
			_bytes_to_read -= mod;
	}

	void				Request::set_UpFile(char *buf, size_t n)
	{
		if (!_file_to_upload)
			_file_to_upload = new UpFile;
		_file_to_upload->append_buf(buf, n);
	}
}

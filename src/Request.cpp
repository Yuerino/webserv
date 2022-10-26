#include "Request.hpp"

namespace webserv
{
	Request::Request() :
		_method(-1),
		_status_code(0),
		_path(),
		_client(),
		_content(),
		_bytes_to_read(0),
		_file_to_upload(NULL),
		_server_listen(),
		_server_name(),
		_server_config(),
		_raw()
	{}

	Request::Request(struct sockaddr_in client_address, Listen server_listen) :
		_method(-1),
		_status_code(0),
		_path(),
		_client(client_address),
		_content(),
		_bytes_to_read(0),
		_file_to_upload(NULL),
		_server_listen(server_listen),
		_server_name(),
		_server_config(),
		_raw()
	{}

	Request::Request(Request const &other) :
		_method(other._method),
		_status_code(other._status_code),
		_path(other._path),
		_client(other._client),
		_content(other._content),
		_bytes_to_read(other._bytes_to_read),
		_file_to_upload(NULL),
		_server_listen(other._server_listen),
		_server_name(other._server_name),
		_server_config(other._server_config),
		_raw(other._raw)
	{
		if (other._file_to_upload)
			_file_to_upload = new UpFile(*other._file_to_upload);
	}

	Request& Request::operator=(Request const &other)
	{
		_method = other._method;
		_status_code = other._status_code;
		_path = other._path;
		_client = other._client;
		_content = other._content;s
		_bytes_to_read = other._bytes_to_read;
		if (other._file_to_upload)
			_file_to_upload = new UpFile(*other._file_to_upload);
		_server_listen = other._server_listen;
		_server_name = other._server_name;
		_server_config = other._server_config;
		_raw = other._raw;
		return *this;
	}

	Request::~Request()
	{
		if (_file_to_upload)
			delete (_file_to_upload);
	}

	void Request::init(std::string const &raw, std::vector<ServerConfig> const &server_configs)
	{
		_raw = raw;
		parse_method();
		parse_path();
		assign_content(request);
		set_server_config(server_configs);
	}

	void Request::parse_method()
	{
		try
		{
			std::string res(_raw.substr(0, _raw.find(" ")));
			int i = 0;
			while (res != HTTPMethodStrings[i] && i < 8)
				i++;
			_method = i;
		}
		catch (const std::exception& e)
		{
			_method = UNKNOWN;
		}
	}

	// TODO parse query here in path
	void Request::parse_path()
	{
		try
		{
			uint32_t	pos = _raw.find(" ") + 1;
			_path = _raw.substr(pos, _raw.find(" ", pos) - pos);
		}
		catch(const std::exception& e)
		{
			_status_code = 400;
		}
	}

	void Request::assign_content()
	{
		size_t	pos(0);
		pos = _raw.find("\r\n", 0);
		while (pos != std::string::npos)
		{
			pos += 2;
			if (_raw.find(":", pos) == std::string::npos)
				break;
			std::string key = _raw.substr(pos, _raw.find(":", pos) - pos);
			_content[key] = parse_field(key);
			pos = _raw.find("\r\n", pos);
		}
	}

	std::string	const Request::parse_field(std::string const &fieldName)
	{
		try
		{
			uint32_t	pos_field = 0;
			pos_field = _raw.find(fieldName);
			std::string	line(_raw.substr(pos_field, _raw.find("\n", pos_field) - pos_field));
			return (line.substr(line.find(" ") + 1));
		}
		catch(const std::exception& e)
		{
			return ("");
		}
	}

	// TODO: move this to init
	void Request::set_bytes_to_read(void)
	{
		if (!_bytes_to_read && _content.find("Content-Length") != _content.end())
			_bytes_to_read = std::atol(_content["Content-Length"].c_str());
	}

	void Request::mod_bytes_to_read(int mod)
	{
		if ((unsigned long)mod > _bytes_to_read)
			_bytes_to_read = 0;
		else
			_bytes_to_read -= mod;
	}

	void Request::set_UpFile(char *buf, size_t n)
	{
		if (!_file_to_upload)
			_file_to_upload = new UpFile;
		_file_to_upload->append_buf(buf, n);
	}

	// TODO: refactor this
	bool Request::check_single_chunk(std::string buffer)
	{
		std::string content = buffer.substr(buffer.find("\r\n\r\n") + 4);

		if (_bytes_to_read && content.size() > 0)
		{
			set_UpFile((char *)content.c_str(), content.size());
			return true;
		}
		return false;
	}

	/**
	 * @brief Set server config and server name accordingly with host header
	 */
	void Request::set_server_config(const std::vector<ServerConfig>& server_configs) {
		std::vector<ServerConfig> possible_server_configs;
		std::vector<ServerConfig>::const_iterator it = server_configs.begin();
		for (; it != server_configs.end(); ++it) {
			if (it->get_listens().count(_server_listen) > 0)
				possible_server_configs.push_back(*it);
		}

		if (possible_server_configs.empty()) {
			_status_code = 404;
			return;
		}

		if (_content.count("Host") == 0) {
			_status_code = 400;
			return;
		}

		std::string host_name = _content.at("Host");
		host_name = host_name.substr(0, host_name.find_first_of(":"));

		std::vector<ServerConfig>::const_iterator s_it = possible_server_configs.begin();
		for (; s_it != possible_server_configs.end(); ++s_it) {
			if (s_it->get_server_names().count(host_name) > 0) {
				_server_name = host_name;
				_server_config = *s_it;
				return;
			}
		}

		_server_name = *possible_server_configs.at(0).get_server_names().begin();
		_server_config = possible_server_configs.at(0);
	}

	int											Request::get_method(void) const { return (_method); }
	int											Request::get_status_code(void) const { return (_status_code); }
	std::string const							&Request::get_path(void) const { return (_path); }
	struct sockaddr_in const					&Request::get_client(void) const { return (_client); }
	std::map<std::string, std::string> const	&Request::get_content(void) const { return (_content); }
	unsigned long const							&Request::get_bytes_to_read(void) const { return (_bytes_to_read); }
	UpFile										*Request::get_UpFile(void) const { return (_file_to_upload); }
	Listen const								&Request::get_server_listen(void) const { return (_server_listen); }
	std::string const							&Request::get_server_name(void) const { return (_server_name); }
	ServerConfig const							&Request::get_server_config(void) const { return (_server_config); }
}

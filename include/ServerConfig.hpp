#pragma once

#include <vector>
#include <string>

namespace webserv {
	class ServerConfig {
	public:
		ServerConfig();
		ServerConfig(const ServerConfig& copy);
		ServerConfig& operator=(const ServerConfig& other);
		~ServerConfig();

		const std::vector<std::string>& get_server_names() const;
		const std::vector<std::string>& get_listens() const;
		const std::string& get_root() const;
		const std::string& get_index() const;
		const std::vector<std::string>& get_allow_methods() const;

		bool set_config(const std::string& type, const std::string& value);

	private:
		std::vector<std::string>	_server_names;
		std::vector<std::string>	_listens;
		std::string					_root;
		std::string					_index;
		std::vector<std::string>	_allow_methods;
		// std::vector<Location>		locations;
	};
} /* namespace webserv */

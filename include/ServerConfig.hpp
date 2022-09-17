#pragma once

#include <ostream>
#include <string>
#include <set>
#include <map>
#include <cstdlib>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <cerrno>
#include <cstring>

#include "LocationConfig.hpp"

namespace webserv {
	struct Listen {
		std::string	address;
		int			port;

		bool operator<(const Listen& other) const;
	};

	class ServerConfig {
	public:
		ServerConfig();
		ServerConfig(const ServerConfig& copy);
		ServerConfig& operator=(const ServerConfig& other);
		~ServerConfig();

		static void register_types(std::set<std::string>& types);
		bool set_config(const std::string& type, const std::string& value);
		bool set_default();
		bool add_location(LocationConfig location_config);

		/* Getters */
		const std::set<std::string>& get_server_names() const;
		const std::set<Listen>& get_listens() const;
		const std::string& get_root() const;
		const std::string& get_index() const;
		const std::set<std::string>& get_allow_methods() const;
		const std::map<std::string, LocationConfig>& get_locations() const;

	private:
		std::set<std::string>					_server_names;
		std::set<Listen>						_listens;
		std::string								_root;
		std::string								_index;
		std::set<std::string>					_allow_methods;
		std::map<std::string, LocationConfig>	_locations;

		bool add_allow_methods(const std::string& method);
		bool add_listen(const std::string& value);
	};

#ifdef PARSER_DEBUG
	std::ostream& operator<<(std::ostream& os, const ServerConfig& server_config);
#endif

} /* namespace webserv */

#pragma once

#include <ostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "LocationConfig.hpp"

namespace webserv {
	class ServerConfig {
	public:
		ServerConfig();
		ServerConfig(const ServerConfig& copy);
		ServerConfig& operator=(const ServerConfig& other);
		~ServerConfig();

		static void register_types(std::set<std::string>& types);
		bool set_config(const std::string& type, const std::string& value);
		bool add_location(LocationConfig location_config);

		/* Getters */
		const std::vector<std::string>& get_server_names() const;
		const std::vector<std::string>& get_listens() const;
		const std::string& get_root() const;
		const std::string& get_index() const;
		const std::set<std::string>& get_allow_methods() const;
		const std::map<std::string, LocationConfig>& get_locations() const;

	private:
		std::vector<std::string>				_server_names;
		std::vector<std::string>				_listens;
		std::string								_root;
		std::string								_index;
		std::set<std::string>					_allow_methods;
		std::map<std::string, LocationConfig>	_locations;
	};

#ifdef PARSER_DEBUG
	std::ostream& operator<<(std::ostream& os, const ServerConfig& server_config);
#endif

} /* namespace webserv */

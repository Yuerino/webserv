#pragma once

#include <ostream>
#include <string>
#include <set>

#include "utils.hpp"

namespace webserv {
	class LocationConfig {
	public:
		LocationConfig();
		LocationConfig(const LocationConfig& copy);
		LocationConfig& operator=(const LocationConfig& other);
		~LocationConfig();

		static void register_types(std::set<std::string>& types);
		bool set_config(const std::string& type, const std::string& value);
		bool set_default();

		/* Getters */
		const std::string& get_location() const;
		const std::string& get_root() const;
		const std::string& get_index() const;
		const std::set<std::string>& get_allow_methods() const;
		const std::string& get_cgi_path() const;

	private:
		std::string				_location;
		std::string				_root;
		std::string				_index;
		std::set<std::string>	_allow_methods;
		std::string				_cgi_path;

		bool add_allow_methods(const std::string& method);
	};

#ifdef PARSER_DEBUG
	std::ostream& operator<<(std::ostream& os, const LocationConfig& location_config);
#endif

} /* namespace webserv */

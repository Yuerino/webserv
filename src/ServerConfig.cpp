#include "ServerConfig.hpp"

namespace webserv {
	ServerConfig::ServerConfig() :
		_server_names(),
		_listens(),
		_root(),
		_index(),
		_allow_methods(),
		_locations() {}

	ServerConfig::ServerConfig(const ServerConfig& copy) :
		_server_names(copy.get_server_names()),
		_listens(copy.get_listens()),
		_root(copy.get_root()),
		_index(copy.get_index()),
		_allow_methods(copy.get_allow_methods()),
		_locations(copy.get_locations()) {}

	ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
		if (this == &other) { return *this; }
		_server_names = other.get_server_names();
		_listens = other.get_listens();
		_root = other.get_root();
		_index = other.get_index();
		_allow_methods = other.get_allow_methods();
		_locations = other.get_locations();
		return *this;
	}

	ServerConfig::~ServerConfig() {}

	/**
	 * @brief Register the avaiable types in ServerConfig for Parser
	 */
	void ServerConfig::register_types(std::set<std::string>& types) {
		types.insert("server_name");
		types.insert("listen");
		types.insert("root");
		types.insert("index");
		types.insert("allow_methods");
		types.insert("location");
	}

	/**
	 * @brief Universal setter
	 * @note If it's a vector type, it appends
	 * @return true if successfully set the value, otherwise false
	 */
	bool ServerConfig::set_config(const std::string& type, const std::string& value) {
		if (type == "server_name")
			_server_names.push_back(value);
		else if (type == "listen")
			_listens.push_back(value);
		else if (type == "root" && _root.empty())
			_root = value;
		else if (type == "index" && _index.empty())
			_index = value;
		else if (type == "allow_methods")
			return _allow_methods.insert(value).second;
		else
			return false;
		return true;
	}

	/**
	 * @brief Add location to server config
	 * @return true if sucessfully add the location, otherwise false
	 */
	bool ServerConfig::add_location(LocationConfig location_config) {
		return _locations.insert(std::make_pair(location_config.get_location(), location_config)).second;
	}

	/* Getters */
	const std::vector<std::string>& ServerConfig::get_server_names() const { return _server_names; }
	const std::vector<std::string>& ServerConfig::get_listens() const { return _listens; }
	const std::string& ServerConfig::get_root() const { return _root; }
	const std::string& ServerConfig::get_index() const { return _index; }
	const std::set<std::string>& ServerConfig::get_allow_methods() const { return _allow_methods; }
	const std::map<std::string, LocationConfig>& ServerConfig::get_locations() const { return _locations; }
} /* namespace webserv */

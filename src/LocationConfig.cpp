#include "LocationConfig.hpp"

namespace webserv {
	LocationConfig::LocationConfig() :
		_location(),
		_root(),
		_index(),
		_allow_methods(),
		_cgi_path() {}

	LocationConfig::LocationConfig(const LocationConfig& copy) :
		_location(copy.get_location()),
		_root(copy.get_root()),
		_index(copy.get_index()),
		_allow_methods(copy.get_allow_methods()),
		_cgi_path(copy.get_cgi_path()) {}

	LocationConfig& LocationConfig::operator=(const LocationConfig& other) {
		if (this == &other) { return *this; }
		_location = other.get_location();
		_root = other.get_root();
		_index = other.get_index();
		_allow_methods = other.get_allow_methods();
		_cgi_path = other.get_cgi_path();
		return *this;
	}

	LocationConfig::~LocationConfig() {}

	/**
	 * @brief Register the avaiable types in LocationConfig for Parser
	 */
	void LocationConfig::register_types(std::set<std::string>& types) {
		types.insert("location");
		types.insert("root");
		types.insert("index");
		types.insert("allow_methods");
		types.insert("cgi_path");
	}

	/**
	 * @brief Universal setter
	 * @note If it's a vector type, it appends
	 * @return True if successfully set the value, otherwise false
	 */
	bool LocationConfig::set_config(const std::string& type, const std::string& value) {
		if (type == "location" && _location.empty())
			_location = value;
		else if (type == "root" && _root.empty())
			_root = value;
		else if (type == "index" && _index.empty())
			_index = value;
		else if (type == "allow_methods")
			return _allow_methods.insert(value).second;
		else if (type == "cgi_path" && _cgi_path.empty())
			_cgi_path = value;
		else
			return false;
		return true;
	}

	/* Getters */
	const std::string& LocationConfig::get_location() const { return _location; }
	const std::string& LocationConfig::get_root() const { return _root; }
	const std::string& LocationConfig::get_index() const { return _index; }
	const std::set<std::string>& LocationConfig::get_allow_methods() const { return _allow_methods; }
	const std::string& LocationConfig::get_cgi_path() const { return _cgi_path; }
} /* namespace webserv */

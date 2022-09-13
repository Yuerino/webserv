#include "ServerConfig.hpp"

namespace webserv {
	ServerConfig::ServerConfig() :
		_server_names(),
		_listens(),
		_root(),
		_index(),
		_allow_methods() {}

	ServerConfig::ServerConfig(const ServerConfig& copy) :
		_server_names(copy.get_server_names()),
		_listens(copy.get_listens()),
		_root(copy.get_root()),
		_index(copy.get_index()),
		_allow_methods(copy.get_allow_methods()) {}

	ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
		if (this == &other) { return *this; }
		this->_server_names = other.get_server_names();
		this->_listens = other.get_listens();
		this->_root = other.get_root();
		this->_index = other.get_index();
		this->_allow_methods = other.get_allow_methods();
		return *this;
	}

	ServerConfig::~ServerConfig() {}

	/**
	 * @brief Universal setter
	 * @note If it's a vector type, it appends
	 * @return True if successfully set the value, otherwise false
	 */
	bool ServerConfig::set_config(const std::string& type, const std::string& value) {
		if (type == "server_name")
			this->_server_names.push_back(value);
		else if (type == "listen")
			this->_listens.push_back(value);
		else if (type == "root" && this->_root.empty())
			this->_root = value;
		else if (type == "index" && this->_index.empty())
			this->_index = value;
		else if (type == "allow_methods")
			this->_allow_methods.push_back(value);
		else
			return false;
		return true;
	}

	/* Getters */
	const std::vector<std::string>& ServerConfig::get_server_names() const { return this->_server_names; }
	const std::vector<std::string>& ServerConfig::get_listens() const { return this->_listens; }
	const std::string& ServerConfig::get_root() const { return this->_root; }
	const std::string& ServerConfig::get_index() const { return this->_index; }
	const std::vector<std::string>& ServerConfig::get_allow_methods() const { return this->_allow_methods; }
} /* namespace webserv */

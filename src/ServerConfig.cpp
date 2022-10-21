#include "ServerConfig.hpp"

namespace webserv {
	/* Struct Listen */

	std::string Listen::get_full_address() const {
		return address + ":" + to_string(port);
	}

	bool Listen::operator<(const Listen& other) const {
		if (address == other.address) {
			return port < other.port;
		}

		return address < other.address;
	}

	/* Class ServerConfig */

	ServerConfig::ServerConfig() :
		_server_names(),
		_listens(),
		_root(),
		_index(),
		_allow_methods(),
		_locations(),
		_client_max_body_size(-1),
		_error_pages() {}

	ServerConfig::ServerConfig(const ServerConfig& copy) :
		_server_names(copy._server_names),
		_listens(copy._listens),
		_root(copy._root),
		_index(copy._index),
		_allow_methods(copy._allow_methods),
		_locations(copy._locations),
		_client_max_body_size(copy._client_max_body_size),
		_error_pages(copy._error_pages) {}

	ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
		if (this == &other) { return *this; }
		_server_names = other._server_names;
		_listens = other._listens;
		_root = other._root;
		_index = other._index;
		_allow_methods = other._allow_methods;
		_locations = other._locations;
		_client_max_body_size = other._client_max_body_size;
		_error_pages = other._error_pages;
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
		types.insert("client_max_body_size");
		types.insert("error_page");
	}

	/**
	 * @brief Universal setter
	 * @note If it's a vector type, it appends
	 * @return true if successfully set the value, otherwise false
	 */
	bool ServerConfig::set_config(const std::string& type, const std::string& value) {
		if (type == "server_name") {
			return _server_names.insert(value).second;
		} else if (type == "listen") {
			return add_listen(value);
		} else if (type == "root" && _root.empty()) {
			_root = value;
		} else if (type == "index" && _index.empty()) {
			_index = value;
		} else if (type == "allow_methods") {
			return add_allow_methods(value);
		} else if (type == "client_max_body_size" && _client_max_body_size == -1 && is_digits(value)) {
			_client_max_body_size = std::atoi(value.c_str());
		} else if (type == "error_page") {
			return add_error_page(value);
		} else {
			return false;
		}

		return true;
	}

	/**
	 * @brief Add location to server config
	 * @return true if sucessfully add the location, otherwise false
	 */
	bool ServerConfig::add_location(LocationConfig location_config) {
		return _locations.insert(std::make_pair(location_config.get_location(), location_config)).second;
	}

	/**
	 * @brief Set the rest of unset configuration to default value
	 * @return true if succesfully set otherwise false
	 */
	bool ServerConfig::set_default() {
		if (_server_names.empty()) {
			_server_names.insert("");
		}

		if (_listens.empty()) {
			Listen listen;
			listen.address = "";
			listen.port = 80;

			_listens.insert(listen);
		}

		if (_root.empty()) {
			_root = "./html";
		}

		if (_index.empty()) {
			_index = "index.html";
		}

		if (_allow_methods.empty()) {
			size_t i = 0;
			size_t size = sizeof(HTTPMethodStrings) / sizeof(const char*);

			for (; i < size; ++i) {
				_allow_methods.insert(HTTPMethodStrings[i]);
			}
		}

		if (_locations.empty()) {
			return false;
		}

		if (!_error_pages.empty() && _error_pages.begin()->second.empty()) {
			return false;
		}

		return true;
	}

	/**
	 * @brief Check if method is valid and add method to allow_methods
	 */
	bool ServerConfig::add_allow_methods(const std::string& method) {
		size_t i = 0;
		size_t size = sizeof(HTTPMethodStrings) / sizeof(const char*);

		for (; i < size; ++i) {
			if (method == HTTPMethodStrings[i])
				break;
		}

		if (i >= size) {
			return false;
		}

		return _allow_methods.insert(method).second;
	}

	/**
	 * @brief Check and add (address:port) to listen
	 */
	bool ServerConfig::add_listen(const std::string& value) {
		Listen listen;
		size_t sep_pos;
		std::string port_str;

		sep_pos = value.find(":");

		if (sep_pos != std::string::npos) {
			listen.address = value.substr(0, sep_pos);
			if (!is_ip4(listen.address)) {
				return false;
			}

			port_str = value.substr(sep_pos + 1);
		} else {
			listen.address = "";
			port_str = value;
		}

		if (!is_digits(port_str)) {
			return false;
		}

		listen.port = std::atoi(port_str.c_str());
		if (listen.port < 1 || listen.port > 65535) {
			return false;
		}

		return _listens.insert(listen).second;
	}

	/**
	 * @brief Check and add error_page to error_pages
	 */
	bool ServerConfig::add_error_page(const std::string& value) {
		if (!_error_pages.empty() && !_error_pages.begin()->second.empty()) {
			return false;
		}

		if (is_digits(value)) {
			int code = std::atoi(value.c_str());

			if (code >= 400 && code < 600) {
				return _error_pages.insert(std::make_pair(value, "")).second;
			}

			return false;
		}

		if (_error_pages.empty()) {
			return false;
		}

		std::map<std::string, std::string>::iterator it = _error_pages.begin();
		for(; it != _error_pages.end(); ++it) {
			it->second = value;
		}

		return true;
	}

	/* Getters */
	const std::set<std::string>& ServerConfig::get_server_names() const { return _server_names; }
	const std::set<Listen>& ServerConfig::get_listens() const { return _listens; }
	const std::string& ServerConfig::get_root() const { return _root; }
	const std::string& ServerConfig::get_index() const { return _index; }
	const std::set<std::string>& ServerConfig::get_allow_methods() const { return _allow_methods; }
	const std::map<std::string, LocationConfig>& ServerConfig::get_locations() const { return _locations; }
	const int& ServerConfig::get_client_max_body_size() const { return _client_max_body_size; }
	const std::map<std::string, std::string>& ServerConfig::get_error_pages() const { return _error_pages; }

#ifdef PARSER_DEBUG
	std::ostream& operator<<(std::ostream& os, const ServerConfig& server_config) {
		os << "server {\n";

		os << "\tserver_name";
		for (std::set<std::string>::const_iterator _it = server_config.get_server_names().begin();
			_it != server_config.get_server_names().end(); ++_it)
			os << " " << *_it;
		os << ";\n";

		os << "\tlisten";
		for (std::set<Listen>::const_iterator _it = server_config.get_listens().begin();
			_it != server_config.get_listens().end(); ++_it)
			os << " " << _it->address << ":" << _it->port;
		os << ";\n";

		os << "\troot " << server_config.get_root() << ";\n";
		os << "\tindex " << server_config.get_index() << ";\n";

		os << "\tallow_methods";
		for (std::set<std::string>::const_iterator _it = server_config.get_allow_methods().begin();
			_it != server_config.get_allow_methods().end(); ++_it)
			os << " " << *_it;
		os << ";\n";

		std::map<std::string, LocationConfig>::const_iterator lit = server_config.get_locations().begin();
		std::map<std::string, LocationConfig>::const_iterator lite = server_config.get_locations().end();

		for (; lit != lite; ++lit) {
			os << lit->second;
		}

		os << "}\n";
		return os;
	}
#endif

} /* namespace webserv */

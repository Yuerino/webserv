#include "ServerConfig.hpp"

namespace webserv {
	/* Struct Listen */

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
		if (_listens.empty()) {
			Listen listen;
			listen.address = "";
			listen.port = 80;

			_listens.insert(listen);
		}

		if (_root.empty()) {
			_root = "http";
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

		sep_pos = value.find(":");

		if (sep_pos == std::string::npos) {
			if (!is_digits(value)) {
				return false;
			}

			listen.port = std::atoi(value.c_str());
			if (listen.port < 1 || listen.port > 65535) {
				return false;
			}

			listen.address = "";
			return _listens.insert(listen).second;
		} else {
			int r;
			unsigned char buffer[sizeof(in_addr)];

			listen.address = value.substr(0, sep_pos);

			r = inet_pton(AF_INET, listen.address.c_str(), buffer);
			if (r == 0) {
				return false;
			} else if (r < 0) {
				LOG_E() << "Fatal error: inet_pton: " << std::strerror(errno) << "\n";
				exit(EXIT_FAILURE);
			}

			std::string port_str = value.substr(sep_pos + 1);
			if (!is_digits(port_str)) {
				return false;
			}

			listen.port = std::atoi(port_str.c_str());
			if (listen.port < 1 || listen.port > 65535) {
				return false;
			}
			return _listens.insert(listen).second;
		}

		return false;
	}

	/* Getters */
	const std::set<std::string>& ServerConfig::get_server_names() const { return _server_names; }
	const std::set<Listen>& ServerConfig::get_listens() const { return _listens; }
	const std::string& ServerConfig::get_root() const { return _root; }
	const std::string& ServerConfig::get_index() const { return _index; }
	const std::set<std::string>& ServerConfig::get_allow_methods() const { return _allow_methods; }
	const std::map<std::string, LocationConfig>& ServerConfig::get_locations() const { return _locations; }

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

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
		if (type == "location" && _location.empty()) {
			_location = value;
		} else if (type == "root" && _root.empty()) {
			_root = value;
		} else if (type == "index" && _index.empty()) {
			_index = value;
		} else if (type == "allow_methods") {
			return add_allow_methods(value);
		} else if (type == "cgi_path" && _cgi_path.empty()) {
			_cgi_path = value;
		} else {
			return false;
		}

		return true;
	}

	/**
	 * @brief Set the rest of unset configuration to default value
	 * @return true if succesfully set otherwise false
	 */
	bool LocationConfig::set_default() {
		if (_location.empty()) {
			return false;
		}

		if (_root.empty()) {
			_root = "html";
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

		return true;
	}

	/**
	 * @brief Check if method is valid and add method to allow_methods
	 */
	bool LocationConfig::add_allow_methods(const std::string& method) {
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

	/* Getters */
	const std::string& LocationConfig::get_location() const { return _location; }
	const std::string& LocationConfig::get_root() const { return _root; }
	const std::string& LocationConfig::get_index() const { return _index; }
	const std::set<std::string>& LocationConfig::get_allow_methods() const { return _allow_methods; }
	const std::string& LocationConfig::get_cgi_path() const { return _cgi_path; }

#ifdef PARSER_DEBUG
	std::ostream& operator<<(std::ostream& os, const LocationConfig& location_config) {
		os << "\tlocation " << location_config.get_location() << " {\n";

		os << "\t\troot " << location_config.get_root() << ";\n";
		os << "\t\tindex " << location_config.get_index() << ";\n";

		os << "\t\tallow_methods";
		for (std::set<std::string>::const_iterator _it = location_config.get_allow_methods().begin();
			_it != location_config.get_allow_methods().end(); ++_it)
			os << " " << *_it;
		os << ";\n";

		os << "\t\tcgi_path " << location_config.get_cgi_path() << ";\n";

		os << "\t}\n";
		return os;
	}
#endif

} /* namespace webserv */

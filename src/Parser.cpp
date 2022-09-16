#include "Parser.hpp"

namespace webserv {
	Parser::Parser() : _tokenizer(), _str(), _tokens(), _scopes_and_types() {
		/* register global scope types */
		_scopes_and_types["global"].insert("server");

		/* register other scope types */
		ServerConfig::register_types(_scopes_and_types["server"]);
		LocationConfig::register_types(_scopes_and_types["location"]);
	}

	Parser::~Parser() {}

	void Parser::parse(const std::string& str_to_parse) {
		_str = str_to_parse;

		_tokens = _tokenizer.tokenize(_str);
		if (_tokens.empty()) {
			throw ParserException("Empty configuration");
		}

		_token_it = _tokens.begin();
		_token_ite = _tokens.end();

		std::vector<ServerConfig> server_configs;
		while (_token_it != _token_ite) {
			internal::Token token = expect_type("global");

			if (token.text == "server") {
				ServerConfig server_config;
				server_config = parse_server_config();
				server_configs.push_back(server_config);
			}
		}

#ifdef PARSER_DEBUG
		// /* debug */ print_tokens();
		/* debug */ print_server_configs(server_configs);
#endif
	}

	/**
	 * @brief Parse server config
	 * @exception Throw ParserException if fail to set server config
	 */
	ServerConfig Parser::parse_server_config() {
		ServerConfig server_config;
		expect_operator("{");

		while (_token_it != _token_ite
			&& (_token_it->type != internal::OPERATOR && _token_it->text != "}")) {
			internal::Token token_type = expect_type("server");

			if (token_type.text == "location") {
				if (!server_config.add_location(parse_location_config())) {
					throw ParserExceptionAtLine("Unexpected token: " + token_type.text, token_type.line_number);
				}
				continue;
			}

			while (_token_it != _token_ite
				&& (_token_it->type != internal::OPERATOR && _token_it->text != ";")) {
				internal::Token token_value = expect_value();

				if (!server_config.set_config(token_type.text, token_value.text)) {
					throw ParserExceptionAtLine("Unexpected token: " + token_value.text, token_value.line_number);
				}
			}

			expect_operator(";");
		}

		expect_operator("}");
		return server_config;
	}

	/**
	 * @brief Parse location config
	 * @exception Throw ParserException if fail to set location config
	 */
	LocationConfig Parser::parse_location_config() {
		LocationConfig location_config;

		internal::Token token_value = expect_value();
		if (!location_config.set_config("location", token_value.text)) {
			throw ParserExceptionAtLine("Unexpected token: " + token_value.text, token_value.line_number);
		}

		expect_operator("{");

		while (_token_it != _token_ite
			&& (_token_it->type != internal::OPERATOR && _token_it->text != "}")) {
			internal::Token token_type = expect_type("location");

			while (_token_it != _token_ite
				&& (_token_it->type != internal::OPERATOR && _token_it->text != ";")) {
				token_value = expect_value();

				if (!location_config.set_config(token_type.text, token_value.text)) {
					throw ParserExceptionAtLine("Unexpected token: " + token_value.text, token_value.line_number);
				}
			}

			expect_operator(";");
		}

		expect_operator("}");
		return location_config;
	}

	/**
	 * @brief Expect and return operator token matched name
	 * @exception Throw ParserException if can't find expected opterator
	 */
	internal::Token Parser::expect_operator(const std::string& name) {
		if (_token_it == _token_ite) {
			throw ParserException("Unexpected end of file, expected: " + name);
		}

		if (_token_it->type != internal::OPERATOR || (!name.empty() && _token_it->text != name)) {
			throw ParserExceptionAtLine("Unexpected token: " + _token_it->text + ", expected: " + name, _token_it->line_number);
		}

		internal::Token token = *_token_it;
		++_token_it;

		return token;
	}

	/**
	 * @brief Expect and return identifier token
	 * @exception Throw ParserException next token isn't an itentifier token
	 */
	internal::Token Parser::expect_value() {
		if (_token_it == _token_ite) {
			throw ParserException("Unexpected end of file");
		}

		if (_token_it->type != internal::IDENTIFIER) {
			throw ParserExceptionAtLine("Unexpected token: " + _token_it->text, _token_it->line_number);
		}

		internal::Token token = *_token_it;
		++_token_it;

		return token;
	}

	/**
	 * @brief Expect and return identifier token that matched types within scope
	 * @exception Throw ParserException if can't find expected type within scope
	 */
	internal::Token Parser::expect_type(const std::string& scope) {
		if (_token_it == _token_ite) {
			throw ParserException("Unexpected end of file");
		}

		if (_token_it->type != internal::IDENTIFIER) {
			throw ParserExceptionAtLine("Unexpected token: " + _token_it->text + " in " + scope + " scope", _token_it->line_number);
		}

		std::map<std::string, std::set<std::string> >::iterator foundScope = _scopes_and_types.find(scope);
		if (foundScope == _scopes_and_types.end()) {
			throw ParserExceptionAtLine("Unexpected scope: " + scope, _token_it->line_number);
		}

		std::set<std::string>::iterator foundType = foundScope->second.find(_token_it->text);
		if (foundType == foundScope->second.end()) {
			throw ParserExceptionAtLine("Unexpected type: " + _token_it->text + " in " + scope + " scope", _token_it->line_number);
		}

		internal::Token token = *_token_it;
		++_token_it;

		return token;
	}

#ifdef PARSER_DEBUG
	/**
	 * @brief Debug function to print all tokens
	 */
	void Parser::print_tokens() const {
		std::vector<Token>::const_iterator it = _tokens.begin();
		std::vector<Token>::const_iterator ite = _tokens.end();

		for (; it != ite; ++it) {
			std::cout << "Token(" << TokenTypeString[it->type] << "): \"" << it->text << "\" at line: ";
			std::cout << it->line_number << "." << std::endl;
		}
	}

	/**
	 * @brief Debug function to print all server_configs
	 */
	void Parser::print_server_configs(const std::vector<ServerConfig>& server_configs) {
		std::vector<ServerConfig>::const_iterator it = server_configs.begin();
		std::vector<ServerConfig>::const_iterator ite = server_configs.end();

		for (; it != ite; ++it) {
			std::cout << "Server config: { ";
			std::cout << "server_name [ ";
			for (std::vector<std::string>::const_iterator _it = it->get_server_names().begin();
				_it != it->get_server_names().end(); ++_it)
				std::cout << *_it << " ";
			std::cout << "], listen [ ";
			for (std::vector<std::string>::const_iterator _it = it->get_listens().begin();
				_it != it->get_listens().end(); ++_it)
				std::cout << *_it << " ";
			std::cout << "], root ";
			std::cout << it->get_root();
			std::cout << ", index ";
			std::cout << it->get_index();
			std::cout << ", allow_methods [ ";
			for (std::set<std::string>::const_iterator _it = it->get_allow_methods().begin();
				_it != it->get_allow_methods().end(); ++_it)
				std::cout << *_it << " ";
			std::cout << "], location [ ";
			Parser::print_location_configs(it->get_locations());
			std::cout << "] }" << std::endl;
		}
	}

	/**
	 * @brief Debug function to print all location_configs
	 */
	void Parser::print_location_configs(const std::map<std::string, LocationConfig>& location_configs) {
		std::map<std::string, LocationConfig>::const_iterator it = location_configs.begin();
		std::map<std::string, LocationConfig>::const_iterator ite = location_configs.end();

		for (; it != ite; ++it) {
			std::cout << "location ";
			std::cout << it->first << " { ";
			std::cout << "root ";
			std::cout << it->second.get_root();
			std::cout << ", index ";
			std::cout << it->second.get_index();
			std::cout << ", allow_methods [ ";
			for (std::set<std::string>::const_iterator _it = it->second.get_allow_methods().begin();
				_it != it->second.get_allow_methods().end(); ++_it)
				std::cout << *_it << " ";
			std::cout << "], cgi_path ";
			std::cout << it->second.get_cgi_path();
			std::cout << " }" << std::endl;
		}
	}
#endif

	ParserException::ParserException(std::string message) throw() : std::invalid_argument(message) {}

	ParserExceptionAtLine::ParserExceptionAtLine(std::string message, size_t line) throw() : ParserException(message), _line(line) {}

	const size_t& ParserExceptionAtLine::get_line() const throw() { return _line; }
} /* namespace webserv */

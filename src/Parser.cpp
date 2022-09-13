#include "Parser.hpp"

namespace webserv {
	template <typename T>
	std::string to_string(const T& t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	Parser::Parser() : _str(), _cursor(0), _tokens(), _scopes_have_types() {
		/* global scope type register*/
		this->_scopes_have_types["global"].insert("server");

		/* server scopetype register */
		this->_scopes_have_types["server"].insert("server_name");
		this->_scopes_have_types["server"].insert("listen");
		this->_scopes_have_types["server"].insert("root");
		this->_scopes_have_types["server"].insert("index");
		this->_scopes_have_types["server"].insert("allow_methods");
	}

	Parser::~Parser() {}

	void Parser::parse(const std::string& str_to_parse) {
		this->_str = str_to_parse;

		this->tokenize();
		if (this->_tokens.empty())
			throw std::runtime_error("Empty configuration.");

		this->_token_it = this->_tokens.begin();
		this->_token_ite = this->_tokens.end();

		std::vector<ServerConfig> server_configs;
		while (this->_token_it != this->_token_ite) {
			Token token = expect_type("global");

			if (token.text == "server") {
				ServerConfig server_config;
				server_config = this->parse_server_config();
				server_configs.push_back(server_config);
			}
		}
		/* debug */ this->print_server_configs(server_configs);
	}

	ServerConfig Parser::parse_server_config() {
		ServerConfig server_config;
		this->expect_operator("{");

		while (this->_token_it != this->_token_ite
			&& (this->_token_it->type != OPERATOR && this->_token_it->text != "}")) {
			Token token_type = expect_type("server");

			while (this->_token_it != this->_token_ite
				&& (this->_token_it->type != OPERATOR && this->_token_it->text != ";")) {
				Token token_value = expect_value();
				if (!server_config.set_config(token_type.text, token_value.text))
					throw std::runtime_error(std::string("Unexpected token:" + token_value.text + " at line " + to_string(token_value.line_number)).c_str());
			}

			this->expect_operator(";");
		}

		this->expect_operator("}");
		return server_config;
	}

	/**
	 * @brief Expect and return operator token matched name
	 * @exception Throw runtime_error if can't find expected opterator
	 */
	Token Parser::expect_operator(const std::string& name) {
		if (this->_token_it == this->_token_ite) {
			throw std::runtime_error(std::string("Unexpected end of file, expected: " + name).c_str());
		}

		if (this->_token_it->type != OPERATOR || (!name.empty() && this->_token_it->text != name)) {
			throw std::runtime_error(std::string("Unexpected token: " + this->_token_it->text + " at line " + to_string(this->_token_it->line_number) + ", expected: " + name).c_str());
		}

		Token token = *this->_token_it;
		++this->_token_it;

		return token;
	}

	/**
	 * @brief Expect and return identifier token
	 * @exception Throw runetime_error next token isn't an itentifier token
	 */
	Token Parser::expect_value() {
		if (this->_token_it == this->_token_ite) {
			throw std::runtime_error("Unexpected end of file");
		}

		if (this->_token_it->type != IDENTIFIER) {
			throw std::runtime_error(std::string("Unexpected token: " + this->_token_it->text + " at line " + to_string(this->_token_it->line_number)).c_str());
		}

		Token token = *this->_token_it;
		++this->_token_it;

		return token;
	}

	/**
	 * @brief Expect and return identifier token that matched types within scope
	 * @exception Throw runtime_error if can't find expected type within scope
	 */
	Token Parser::expect_type(const std::string& scope) {
		if (this->_token_it == this->_token_ite) {
			throw std::runtime_error("Unexpected end of file");
		}

		if (this->_token_it->type != IDENTIFIER) {
			throw std::runtime_error(std::string("Unexpected token: " + this->_token_it->text + " at line " + to_string(this->_token_it->line_number)).c_str());
		}

		std::map<std::string, std::set<std::string> >::iterator foundScope = this->_scopes_have_types.find(scope);
		if (foundScope == this->_scopes_have_types.end())
			throw std::runtime_error("Unexpected scope");

		std::set<std::string>::iterator foundType = foundScope->second.find(this->_token_it->text);
		if (foundType == foundScope->second.end())
			throw std::runtime_error("Unexpected type");

		Token token = *this->_token_it;
		++this->_token_it;

		return token;
	}

	/**
	 * @brief Tokenizer
	 */
	void Parser::tokenize() {
		if (this->_str.empty()) { return; }

		Token current_token;
		current_token.text = "";
		current_token.type = WHITESPACE;
		current_token.line_number = 1;

		for (; this->_cursor < this->_str.length(); ++this->_cursor) {
			char current_char = this->_str[this->_cursor];
			switch (current_char) {
				case ' ':
				case '\t':
					if (current_token.type == COMMENT)
						current_token.text.append(1, current_char);
					else
						this->add_token(current_token);
					break;
				case '\r':
				case '\n':
					this->add_token(current_token);
					++current_token.line_number;
					break;
				case '#':
					this->add_token(current_token);
					current_token.type = COMMENT;
					break;
				case '{':
				case '}':
				case ';':
					if (current_token.type != COMMENT) {
						this->add_token(current_token);
						current_token.type = OPERATOR;
						current_token.text.append(1, current_char);
						this->add_token(current_token);
					} else {
						current_token.text.append(1, current_char);
					}
					break;
				default:
					if (current_token.type == WHITESPACE) {
						this->add_token(current_token);
						current_token.type = IDENTIFIER;
					}
					current_token.text.append(1, current_char);
			}
		}
		this->add_token(current_token);
	}

	/**
	 * @brief Add current_token to vector of tokens then reset current_token to
	 * parse next token. This ignore whitespace and comment token.
	 */
	void Parser::add_token(Token &current_token) {
		if (current_token.type != WHITESPACE && current_token.type != COMMENT) {
			this->_tokens.push_back(current_token);
		}

		current_token.text.erase();
		current_token.type = WHITESPACE;
	}

#ifdef TOKEN_DEBUG
	/**
	 * @brief Debug function to print all tokens
	 */
	void Parser::print_tokens() const {
		std::vector<Token>::const_iterator it = this->_tokens.begin();
		std::vector<Token>::const_iterator ite = this->_tokens.end();

		for (; it != ite; ++it) {
			std::cout << "Token(" << TokenTypeString[it->type] << "): \"" << it->text << "\" at line: ";
			std::cout << it->line_number << "." << std::endl;
		}
	}

	void Parser::print_server_configs(const std::vector<ServerConfig>& server_configs) const {
		std::vector<ServerConfig>::const_iterator it = server_configs.begin();
		std::vector<ServerConfig>::const_iterator ite = server_configs.end();

		for (; it != ite; ++it) {
			std::cout << "Server config: {";
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
			for (std::vector<std::string>::const_iterator _it = it->get_allow_methods().begin();
				_it != it->get_allow_methods().end(); ++_it)
				std::cout << *_it << " ";
			std::cout << "]" << std::endl;
		}
	}
#endif
} /* namespace webserv */

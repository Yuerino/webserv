#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <map>
#include <set>
#include <sstream>

#include "Token.hpp"
#include "ServerConfig.hpp"

namespace webserv {
	class Parser {
	public:
		Parser();
		~Parser();

		void parse(const std::string& str_to_parse);

	private:
		ServerConfig parse_server_config();

		Token expect_operator(const std::string& name);

		Token expect_value();

		Token expect_type(const std::string& scope);

		void tokenize();

		void add_token(Token &current_token);

#ifdef TOKEN_DEBUG
		void print_tokens() const;

		void print_server_configs(const std::vector<ServerConfig>& server_configs) const;
#endif

	private:
		std::string			_str;
		size_t 				_cursor;
		std::vector<Token>	_tokens;

		std::map<std::string, std::set<std::string> >	_scopes_have_types;

		std::vector<Token>::iterator	_token_it;
		std::vector<Token>::iterator	_token_ite;

		Parser(const Parser& copy); /* disabled */
		Parser& operator=(const Parser& other); /* disabled */
	};
} /* namespace webserv */

#pragma once

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>

#include "utils.hpp"
#include "Tokenizer.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

namespace webserv {
	class Parser {
	public:
		Parser();
		~Parser();

		void parse(const std::string& str_to_parse);

	private:
		ServerConfig parse_server_config();

		LocationConfig parse_location_config();

		internal::Token expect_operator(const std::string& name);

		internal::Token expect_value();

		internal::Token expect_type(const std::string& scope);

#ifdef PARSER_DEBUG
		void print_tokens() const ;

		static void print_server_configs(const std::vector<ServerConfig>& server_configs);

		static void print_location_configs(const std::map<std::string, LocationConfig>& location_configs);
#endif

	private:
		internal::Tokenizer				_tokenizer;

		std::string						_str;
		std::vector<internal::Token>	_tokens;

		std::map<std::string, std::set<std::string> >	_scopes_and_types;

		std::vector<internal::Token>::iterator	_token_it;
		std::vector<internal::Token>::iterator	_token_ite;

		Parser(const Parser& copy); /* disabled */
		Parser& operator=(const Parser& other); /* disabled */
	};

	class ParserException : public std::invalid_argument {
	public:
		ParserException(std::string message) throw();
	};

	class ParserExceptionAtLine : public ParserException {
	public:
		ParserExceptionAtLine(std::string message, size_t line) throw();

		const size_t& get_line() const throw();
	protected:
		size_t	_line;
	};
} /* namespace webserv */

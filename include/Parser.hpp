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

	namespace internal {

#ifdef PARSER_DEBUG
		/**
		 * @brief Debug function to print any vector<T>
		 * @note As long as T type has operator<< for ostream overloaded
		 */
		template <typename T>
		static void print_debug_vector(const std::vector<T>& v) {
			typename std::vector<T>::const_iterator it = v.begin();
			typename std::vector<T>::const_iterator ite = v.end();

			for (; it != ite; ++it) {
				LOG_D() << *it << "\n";
			}
		}
#endif

	} /* namespace intenral */
} /* namespace webserv */

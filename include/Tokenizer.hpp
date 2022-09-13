#pragma once

#include <vector>
#include "Token.hpp"

namespace webserv {
	class Tokenizer {
	public:
		Tokenizer();
		~Tokenizer();

		std::vector<Token> tokenize(const std::string& str_to_parse);

	private:
		std::string			_str;
		size_t 				_cursor;
		Token				_current_token;
		std::vector<Token>	_tokens;

		void add_current_token();

		Tokenizer(const Tokenizer& copy); /* disabled */
		Tokenizer& operator=(const Tokenizer& other); /* disabled */
	};
} /* namespace webserv */

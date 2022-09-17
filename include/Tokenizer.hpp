#pragma once

#include <ostream>
#include <string>
#include <vector>
#include <iomanip>

namespace webserv {
	namespace internal {
		enum TokenType {
			WHITESPACE, /* Internal usage */
			COMMENT, /* Internal usage */
			IDENTIFIER,
			OPERATOR
		};

		static const char* const TokenTypeString[] = {
			"WHITESPACE",
			"COMMENT",
			"IDENTIFIER",
			"OPERATOR"
		};

		struct Token {
			enum TokenType	type;
			std::string		text;
			size_t			line_number;
		};

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

#ifdef PARSER_DEBUG
		std::ostream& operator<<(std::ostream& os, const Token& token);
#endif

	} /* namespace internal */
} /* namespace webserv */

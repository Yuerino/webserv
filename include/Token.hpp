#pragma once

#include <string>

namespace webserv {
	enum TokenType {
		WHITESPACE, /* Internal usage */
		COMMENT, /* Internal usage */
		IDENTIFIER,
		OPERATOR
	};

	static const char * const TokenTypeString[] = {
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
} /* namespace webserv */


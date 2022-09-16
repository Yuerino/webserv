#include "Tokenizer.hpp"

namespace webserv {
	namespace internal {
		Tokenizer::Tokenizer() : _str(), _cursor(0), _current_token(), _tokens() {}

		Tokenizer::~Tokenizer() {}

		/**
		 * @brief Tokenize the string
		 */
		std::vector<Token> Tokenizer::tokenize(const std::string& str_to_parse) {
			if (str_to_parse.empty()) { return _tokens; }
			_str = str_to_parse;

			_current_token.text.erase();
			_current_token.type = WHITESPACE;
			_current_token.line_number = 1;

			for (_cursor = 0; _cursor < str_to_parse.length(); ++_cursor) {
				char current_char = _str[_cursor];
				switch (current_char) {
					case ' ':
					case '\t':
						if (_current_token.type == COMMENT) {
							_current_token.text.append(1, current_char);
						} else {
							add_current_token();
						}
						break;
					case '\r':
					case '\n':
						add_current_token();
						++_current_token.line_number;
						break;
					case '#':
						add_current_token();
						_current_token.type = COMMENT;
						break;
					case '{':
					case '}':
					case ';':
						if (_current_token.type != COMMENT) {
							add_current_token();
							_current_token.type = OPERATOR;
							_current_token.text.append(1, current_char);
							add_current_token();
						} else {
							_current_token.text.append(1, current_char);
						}
						break;
					default:
						if (_current_token.type == WHITESPACE) {
							add_current_token();
							_current_token.type = IDENTIFIER;
						}
						_current_token.text.append(1, current_char);
				}
			}
			add_current_token();
			return _tokens;
		}

		/**
		 * @brief Add current_token to vector of tokens then reset current_token to
		 * parse next token. This ignore whitespace and comment token.
		 */
		void Tokenizer::add_current_token() {
			if (_current_token.type != WHITESPACE && _current_token.type != COMMENT) {
				_tokens.push_back(_current_token);
			}

			_current_token.text.erase();
			_current_token.type = WHITESPACE;
		}
	} /* namespace internal */
} /* namespace webserv */

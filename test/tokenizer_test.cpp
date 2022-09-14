#include "gtest/gtest.h"
#include <vector>
#include <string>

#include "utils.hpp"
#include "Tokenizer.hpp"
#include "Token.hpp"

namespace webserv {

TEST(TokenizerTest, ValidTest)
{
	std::vector<std::string> expected{"server", "{", "server_name", "youpi", ";", "listen", "0.0.0.0:8000", ";", "root", "./YoupiBanane", ";", "index", "index.html", ";", "allow_methods", "GET", ";", "location", "/put_test", "{", "allow_methods", "PUT", ";", "root", "./YoupiBanane/put_here", ";", "}", "location", "/post_body", "{", "allow_methods", "POST", ";", "}", "location", "/directory", "{", "allow_methods", "GET", ";", "root", "YoupiBanane/", ";", "index", "youpi.bad_extension", ";", "}", "location", "*.bla", "{", "allow_methods", "POST", ";", "cgi_path", "test_linux/ubuntu_cgi_tester", ";", "}", "}"};
	std::vector<std::string>::iterator e_it = expected.begin();
	std::vector<std::string>::iterator e_ite = expected.end();

	Tokenizer tokenizer;

	std::vector<Token> tokens = tokenizer.tokenize(file_to_string("./test/config/tokenizer_test.conf"));

	std::vector<Token>::iterator t_it = tokens.begin();
	std::vector<Token>::iterator t_ite = tokens.end();

	for (; t_it != t_ite && e_it != e_ite; ++t_it, ++e_it) {
		EXPECT_EQ(t_it->text, *e_it);
	}

	if (t_it != t_ite) {
		FAIL() << "Has too much tokens";
	}
	else if (e_it != e_ite) {
		FAIL() << "Expected more tokens";
	}
};

} /* namespace webserv */

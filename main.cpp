#include <iostream>
#include <stdexcept>
#include "utils.hpp"
#include "Parser.hpp"

int main() {
	webserv::Parser parser;

	try {
		parser.parse(webserv::file_to_string("./config/default.conf"));
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

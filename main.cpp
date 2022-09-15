#include <iostream>
#include <stdexcept>
#include <stdlib.h>

#include "utils.hpp"
#include "Parser.hpp"

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << RED << "Invalid number of arguments." << RESET << std::endl;
		return EXIT_FAILURE;
	}

	webserv::Parser parser;

	try {
		parser.parse(webserv::file_to_string(argv[1]));
	} catch (const std::exception& e) {
		std::cerr << RED << e.what() << RESET << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

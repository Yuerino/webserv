#include <iostream>
#include <stdexcept>
#include <stdlib.h>

#include "utils.hpp"
#include "Parser.hpp"
#include "Logger.hpp"

int main(int argc, char **argv) {
	if (argc != 2) {
		LOG_E() << "Invalid number of arguments.\n";
		return EXIT_FAILURE;
	}

	webserv::Parser parser;

	try {
		parser.parse(webserv::file_to_string(argv[1]));
	} catch (const std::exception& e) {
		LOG_E() << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

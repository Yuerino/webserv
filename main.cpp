#include <iostream>
#include <stdexcept>
#include <stdlib.h>

#include "utils.hpp"
#include "Parser.hpp"
#include "Logger.hpp"

int main(int argc, char **argv) {
	if (argc != 2) {
		LOG_E() << "Invalid number of arguments. Usage: ./webserv [ config file ]\n";
		return EXIT_FAILURE;
	}

	if (!webserv::is_valid_extension(argv[1], ".conf")) {
		LOG_E() << "Invalid configuration file extension.\n";
		return EXIT_FAILURE;
	}

	webserv::Parser parser;
	try {
		parser.parse(webserv::file_to_string(argv[1]));
	} catch (const webserv::ParserExceptionAtLine& e) {
		LOG_E() << "[" << argv[1] << ":" << e.get_line() << "] " << e.what() << "\n";
		return EXIT_FAILURE;
	} catch (const webserv::ParserException& e) {
		LOG_E() << "[" << argv[1] << "] " << e.what() << "\n";
		return EXIT_FAILURE;
	} catch (const std::exception& e) {
		LOG_E() << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

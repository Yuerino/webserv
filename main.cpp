#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#ifdef DEBUG
#define PARSER_DEBUG
#endif

#include "utils.hpp"
#include "Parser.hpp"
#include "Server.hpp"

void sig_handler(int num) {
	if (num == SIGINT || num == SIGQUIT) {
		LOG_I() << "Gracefully shutting down server...\n";
		webserv::internal::g_shutdown = true;
	}
}

int main(int argc, char **argv) {
	LOG_FILE("webserv.log");

	if (argc != 2) {
		LOG_E() << "Invalid number of arguments. Usage: ./webserv [ config file ]\n";
		return EXIT_FAILURE;
	}

	if (!webserv::is_extension(argv[1], ".conf")) {
		LOG_E() << "Invalid configuration file extension.\n";
		return EXIT_FAILURE;
	}

	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);

	webserv::Parser parser;
	try {
		webserv::Server server(parser.parse(webserv::file_to_string(argv[1])));
		server.init();
		server.run();
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

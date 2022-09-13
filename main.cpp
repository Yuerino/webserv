#include <iostream>
#include <fstream>
#include <stdexcept>
#include "Parser.hpp"

int main() {
	webserv::Parser parser;
	std::string str_to_parse;

	std::ifstream infile("/Users/cthien-h/42/webserv-project/webserv/config/default.conf", std::ios::in | std::ios::ate);
	if (!infile.is_open()) {
		std::cerr << "Failed to open file." << std::endl;
		return 1;
	}
	str_to_parse.resize(infile.tellg());
	infile.seekg(0, std::ios::beg);
	infile.read(&str_to_parse[0], str_to_parse.size());
	infile.close();

	try {
		parser.parse(str_to_parse);
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

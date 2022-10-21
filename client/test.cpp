#include "client.hpp"

int main (int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: ./client <ip> <port>\n";
		return 1;
	}
	std::string ip = argv[1];
	int port = std::stoi(argv[2]);
	Client client(ip, port);
	return 0;
}
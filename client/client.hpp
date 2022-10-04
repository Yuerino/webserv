#pragma once

#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <string>
#include <iostream>

class Client {
public:
	Client(std::string ip, int port);
	~Client();
private:
	char buf[4096];
	int socket_fd;
	void init(std::string ip, int port);
	void run();
	static int create_socket();
	static void bind_socket(const int& socket_fd, const std::string& host, const int& port);
	void connect_sock(int socket_fd, sockaddr_in sock_addr);
};

// Client.hpp
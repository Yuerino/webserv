#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
	// Create socket
	int sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFd == -1) {
		std::cerr << "Failed to create socket. errno: " << errno << std::endl;
		return (EXIT_FAILURE);
	}

	// Setup socket address/port and options
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	// listen on any address
	// https://man7.org/linux/man-pages/man3/inet_pton.3.html later usage
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	// htons is necessary to convert a number to network byte order
	sockAddr.sin_port = htons(4242);

	// Bind socket
	if (bind(sockFd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0) {
		std::cerr << "Failed to bind. errno: " << errno << std::endl;
		close(sockFd);
		return (EXIT_FAILURE);
	}

	// Listen to socket, 10 concurrent connection
	if (listen(sockFd, 10) < 0) {
		std::cerr << "Failed to listen on socket. errno: " << errno << std::endl;
		close(sockFd);
		return (EXIT_FAILURE);
	}
	std::cout << "HTTP server read for connection" << std::endl;

	// Accept connection
	socklen_t addrLen = sizeof(sockAddr);
	int connectionFd = accept(sockFd, (struct sockaddr*)&sockAddr, &addrLen);
	if (connectionFd < 0) {
		std::cout << "Failed to accept connection. errno: " << errno << std::endl;
		close(sockFd);
		return (EXIT_FAILURE);
	}
	std::cout << "Accepted a connection" << std::endl;

	// Read from the connection
	char buffer[2048];
	ssize_t bytesRead = recv(connectionFd, buffer, 2048, 0);
	if (bytesRead < 0) {
		std::cout << "Failed to read data. errno: " << errno << std::endl;
		close(connectionFd);
		close(sockFd);
		return (EXIT_FAILURE);
	} else if (bytesRead == 0) {
		std::cout << "Connection closed" << std::endl;
		close(connectionFd);
		close(sockFd);
		return (EXIT_FAILURE);
	} else {
		std::cout << "The message was: " << buffer << std::endl;
	}
	std::cout << "Received a message from connection" << std::endl;

	// Send a response
	std::string response = "Hello World 42!\n";
	if (send(connectionFd, response.c_str(), response.size(), 0) == -1) {
		std::cout << "Failed to send the response. errno: " << errno << std::endl;
		close(connectionFd);
		close(sockFd);
		return (EXIT_FAILURE);
	}
	std::cout << "Send a response to connection" << std::endl;

	// Close the connections

	// https://serverfault.com/questions/329845/how-to-forcibly-close-a-socket-in-time-wait
	// Note: doesn't close TCP connection immediately but after a TIME-WAIT
	// peroid because how TCP works (TLDR: it sends shutdown signal and waiting
	// acknowledge from client) so running this program again right after will
	// return errno Address already in use.

	// A way to resolve this issue is to set the socket option to be able to
	// rebind the address and port. (setsockopt())
	close(connectionFd);
	close(sockFd);

	// To test socket, run telent localhost 4242
	return 0;
}

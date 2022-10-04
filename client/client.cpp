#include "client.hpp"

Client::Client(std::string ip, int port) {
	init(ip, port);
	run();
}

/**
 * @brief close socket
 */
Client::~Client() {
	if (socket_fd > 0) {
		close(socket_fd);
	}
}

/**
 * @brief Create and set up socket for all listens
 */
void Client::init(std::string ip, int port) {
	int socket_fd = create_socket();
	sockaddr_in sock_addr = bind_socket(socket_fd, ip, port);
	connect_sock(socket_fd, sock_addr);
}

void Client::run() {
	while (1) {
		std::string msg;
		std::cout << "> ";
		std::getline(std::cin, msg);
		if (msg == "exit") {
			break;
		}
		int send_res = send(socket_fd, msg.c_str(), msg.size() + 1, 0);
		if (send_res == -1) {
			std::cerr << "Fail to send message\n";
		}
		//LOG_D() << "Send message: " << msg << "\n";
		memset(buf, 0, sizeof(buf));
		int recv_res = recv(socket_fd, buf, sizeof(buf), 0);
		if (recv_res == -1) {
			std::cerr << "Fail to receive message\n";
		}
		//LOG_D() << "Receive message: " << buf << "\n";
		std::cout << "Server: " << std::string(buf, recv_res) << endl;
	}
}

/**
 * @brief Create a socket
 * @throw runtime_error in case fail to create
 */
int Client::create_socket() {
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		throw std::runtime_error("Fail to create socket: " + std::string(std::strerror(errno)) + "\n");
	}
	//LOG_D() << "Create a socket, fd: " << socket_fd << "\n";

	return socket_fd;
}

/**
 * @brief connect socket
 * @throw runtime_error in case fail to connect
 */
void Client::connect_sock(int socket_fd, sockaddr_in sock_addr) {
	int connect_res = connect(socket_fd, (sockaddr *)&sock_addr, sizeof(sock_addr));
	if (connect_res == -1) {
		throw std::runtime_error("Fail to connect socket: " + std::string(std::strerror(errno)) + "\n");
	}
	//LOG_D() << "Connect to server, fd: " << connect_res << "\n";
}

/**
 * @brief Setup socket option and bind it to host:port
 */
sockaddr_in Client::bind_socket(const int& socket_fd, const std::string& host, const int& port) {
	int _ = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &_, sizeof(_)) == -1) {
		close(socket_fd);
		throw std::runtime_error("Fail to set socket option: " + std::string(std::strerror(errno)) + "\n");
	}
	//LOG_D() << "Set options for socket fd: " << socket_fd << "\n";

	sockaddr_in sock_addr;
	bzero(&sock_addr, sizeof(sock_addr));

	sock_addr.sin_family = AF_INET;
	if (host.empty()) {
		sock_addr.sin_addr.s_addr = INADDR_ANY;
	} else {
		inet_pton(AF_INET, host.c_str(), &sock_addr.sin_addr.s_addr);
	}

	sock_addr.sin_port = htons(port);

	if (bind(socket_fd, (sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
		close(socket_fd);
		throw std::runtime_error("Failed to bind socket: " + std::string(std::strerror(errno)) + "\n");
	}
	//LOG_D() << "Bind socket fd: " << socket_fd << " to " << host << ":" << port << "\n";
}
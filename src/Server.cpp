#include "Server.hpp"

namespace webserv {
	Server::Server(const std::vector<ServerConfig>& server_configs) : _server_configs(server_configs), _iohandler() {
		std::vector<ServerConfig>::const_iterator s_it = _server_configs.begin();
		std::vector<ServerConfig>::const_iterator s_ite = _server_configs.end();

		for (; s_it != s_ite; ++s_it) {
			_listens.insert(s_it->get_listens().begin(), s_it->get_listens().end());
		}
	}

	/**
	 * @brief Close all open socket and client fds
	 */
	Server::~Server() {
		for (socket_it = _socket_fds.begin(); socket_it != _socket_fds.end(); ++socket_it) {
			if (socket_it->first > 0) {
				close(socket_it->first);
				LOG_D() << "Close socket " << socket_it->second.get_full_address() << ", fd: <<" << socket_it->first << "\n";
			}
		}
	}

	/**
	 * @brief Create and set up socket for all listens
	 */
	void Server::init() {
		if (_iohandler.get_poll_fd() == -1) {
			throw std::runtime_error("Fail to create poll: " + std::string(std::strerror(errno)) + "\n");
		}

		int socket_fd;
		std::set<Listen>::const_iterator l_it = _listens.begin();

		for (; l_it != _listens.end(); ++l_it) {
			socket_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (socket_fd == -1) {
				throw std::runtime_error("Fail to create socket: " + std::string(std::strerror(errno)) + "\n");
			}
			LOG_D() << "Create a socket, fd: " << socket_fd << "\n";

			int _ = 1;
			if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &_, sizeof(_)) == -1) {
				close(socket_fd);
				throw std::runtime_error("Fail to set socket option: " + std::string(std::strerror(errno)) + "\n");
			}
			LOG_D() << "Set options for socket fd: " << socket_fd << "\n";

			sockaddr_in sock_addr;
			bzero(&sock_addr, sizeof(sock_addr));
			sock_addr.sin_family = AF_INET;
			if (l_it->address.empty()) {
				sock_addr.sin_addr.s_addr = INADDR_ANY;
			} else {
				inet_pton(AF_INET, l_it->address.c_str(), &sock_addr.sin_addr.s_addr);
			}
			sock_addr.sin_port = htons(l_it->port);

			if (bind(socket_fd, (sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
				close(socket_fd);
				throw std::runtime_error("Failed to bind socket: " + std::string(std::strerror(errno)) + "\n");
			}
			LOG_D() << "Bind socket fd: " << socket_fd << " to " << l_it->get_full_address() << "\n";

			_iohandler.add_fd(socket_fd);
			LOG_D() << "Add socket fd: " << socket_fd << " to kevent\n";

			_socket_fds.insert(std::make_pair(socket_fd, *l_it));
		}
	}

	/**
	 * @brief listen for connections
	 */
	void Server::run() {
		for (socket_it = _socket_fds.begin(); socket_it != _socket_fds.end(); ++socket_it) {
			if (listen(socket_it->first, 100) == -1) {
				throw std::runtime_error("Fail to listen from socket fd: " + std::string(std::strerror(errno)) + "\n");
			}
			LOG_D() << "Listening for connection on socket fd: " << socket_it->first << "\n";
		}

		int new_event_size;
		int triggered_fd;
		while (69) {
			new_event_size = _iohandler.wait_for_new_event();

			for (int i = 0; i < new_event_size; ++i) {
				triggered_fd = _iohandler.get_triggered_fd(i);
				if (_iohandler.is_error(i)) {
					LOG_E() << "Poll failed: " << std::string(std::strerror(errno)) << "\n";
					if (_socket_fds.find(triggered_fd) != _socket_fds.end()) {
						_socket_fds.erase(triggered_fd);
					}
					close(triggered_fd);
				} else if (_iohandler.is_eof(i)) {
					LOG_I() << "Client fd: " << triggered_fd << " left server.\n";
					_iohandler.remove_fd(triggered_fd);
					close(triggered_fd);
				} else if (_socket_fds.find(triggered_fd) != _socket_fds.end()) {
					int client_fd = accept(triggered_fd, NULL, NULL);
					if (client_fd == -1) {
						LOG_E() << "Failed to accept connection: " << std::strerror(errno) << "\n";
					} else {
						LOG_I() << "Accepted a connection\n";
						_iohandler.add_fd(client_fd);
					}
				} else if (_iohandler.is_read_ready(i)) {
					char buffer[2048];
					ssize_t bytesRead = recv(triggered_fd, buffer, 2048, 0);
					if (bytesRead == -1 || bytesRead == 0) {
						LOG_E() << "Failed to read data.\n";
						_iohandler.remove_fd(triggered_fd);
						close(triggered_fd);
					} else {
						buffer[bytesRead] = '\0';
						LOG_I() << "Received a message from connection: " << buffer << "\n";
						_iohandler.set_write_ready(triggered_fd);
					}
				} else if (_iohandler.is_write_ready(i)) {
					std::string response_header =	"HTTP/1.1 200 OK\r\n"
													"Date: Mon, 01 Jul 2022 12:12:12 GMT\r\n"
													"Server: webserv\r\n"
													"Content-Length: 88\r\n"
													"Content-Type: text/html\r\n"
													"Connection: close\r\n\r\n"
													"<html>\n"
													"<body>\n"
													"<h1>Hello, World!</h1>\n"
													"</body>\n"
													"</html>";

					int ret = send(triggered_fd, response_header.c_str(), response_header.size(), 0);
					if (ret == -1 || ret == 0) {
						LOG_E() << "Failed to send the response header.\n";
					} else {
						LOG_I() << "Send a response to client\n";
					}
					_iohandler.remove_fd(triggered_fd);
					close(triggered_fd);
				} else {
					LOG_E() << "Client disconnected.\n";
					_iohandler.remove_fd(triggered_fd);
					close(triggered_fd);
				}
			}
		}
	}
} /* namespace webserv */




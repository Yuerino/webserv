#include "Server.hpp"
#include "Request.hpp"

namespace webserv {
	bool internal::g_shutdown = false;

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
			}
		}

		std::map<int, Request>::iterator client_it = _clients.begin();
		for (; client_it != _clients.end(); ++client_it) {
			if (client_it->first > 0) {
				close(client_it->first);
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
			socket_fd = create_socket();

			bind_socket(socket_fd, l_it->address, l_it->port);

			_iohandler.add_fd(socket_fd);
			LOG_D() << "Add socket fd: " << socket_fd << " to kevent\n";

			_socket_fds.insert(std::make_pair(socket_fd, *l_it));
		}
	}

	/**
	 * @brief Create a socket
	 * @throw runtime_error in case fail to create
	 */
	int Server::create_socket() {
		int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1) {
			throw std::runtime_error("Fail to create socket: " + std::string(std::strerror(errno)) + "\n");
		}
		LOG_D() << "Create a socket, fd: " << socket_fd << "\n";

		return socket_fd;
	}

	/**
	 * @brief Setup socket option and bind it to host:port
	 */
	void Server::bind_socket(const int& socket_fd, const std::string& host, const int& port) {
		int _ = 1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &_, sizeof(_)) == -1) {
			close(socket_fd);
			throw std::runtime_error("Fail to set socket option: " + std::string(std::strerror(errno)) + "\n");
		}
		LOG_D() << "Set options for socket fd: " << socket_fd << "\n";

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
		LOG_D() << "Bind socket fd: " << socket_fd << " to " << host << ":" << port << "\n";
	}

	/**
	 * @brief listen for connections
	 */
	void Server::run() {
		for (socket_it = _socket_fds.begin(); socket_it != _socket_fds.end(); ++socket_it) {
			if (listen(socket_it->first, 200) == -1) {
				throw std::runtime_error("Fail to listen from socket fd: " + std::string(std::strerror(errno)) + "\n");
			}
			LOG_D() << "Listening for connection on socket fd: " << socket_it->first << "\n";
		}

		int new_event_size;
		int triggered_fd;
		while (!internal::g_shutdown) {
			new_event_size = _iohandler.wait_for_new_event();

			for (int i = 0; i < new_event_size; ++i) {
				triggered_fd = _iohandler.get_triggered_fd(i);
				if (_iohandler.is_error(i)) {
					handle_fail_event(triggered_fd);
				} else if (_iohandler.is_eof(i)) {
					remove_client(triggered_fd);
				} else if (_socket_fds.count(triggered_fd) != 0) {
					handle_accept_client(triggered_fd);
				} else if (_iohandler.is_read_ready(i)) {
					handle_read_event(triggered_fd);
				} else if (_iohandler.is_write_ready(i)) {
					handle_write_event(triggered_fd);
				} else {
					LOG_E() << "Unknown poll event\n";
					remove_client(triggered_fd);
				}
			}
		}
	}

	/**
	 * @brief Remove client from server
	 */
	void Server::remove_client(const int& client_fd) {
		LOG_D() << "Removed client fd: " << client_fd << "\n";

		_iohandler.remove_fd(client_fd);

		if (_clients.count(client_fd) > 0) {
			_clients.erase(client_fd);
		}

		if (client_fd > 0) {
			close(client_fd);
		}
	}

	/**
	 * @brief Handle fail poll event
	 */
	void Server::handle_fail_event(const int& triggered_fd) {
		LOG_E() << "Poll failed: " << std::string(std::strerror(errno)) << "\n";

		_iohandler.remove_fd(triggered_fd);

		if (_socket_fds.count(triggered_fd) > 0) {
			_socket_fds.erase(triggered_fd);
		} else if (_clients.count(triggered_fd) > 0) {
			_clients.erase(triggered_fd);
		}

		if (triggered_fd > 0) {
			close(triggered_fd);
		}
	}

	/**
	 * @brief Handle socket accept connection from client
	 */
	void Server::handle_accept_client(const int& socket_fd) {
		struct sockaddr_in	client_address;
		bzero(&client_address, sizeof(client_address));
		socklen_t address_len= sizeof(client_address);

		int client_fd = accept(socket_fd, (struct sockaddr*)&client_address, &address_len);

		if (client_fd == -1) {
			LOG_E() << "Failed to accept connection: " << std::strerror(errno) << "\n";
			return;
		}

		LOG_I() << "Accepted a connection, client fd: " << client_fd << "\n";

		_iohandler.add_fd(client_fd);

		if (_socket_fds.count(socket_fd) == 0) {
			LOG_E() << "Socket fd :" << socket_fd << "somehow not in server list\n";
			return;
		}

		if (_clients.count(client_fd) == 0) {
			_clients.insert(std::make_pair(client_fd, Request(client_address, _socket_fds.find(socket_fd)->second)));
		} else {
			LOG_E() << "Client fd: " << client_fd << " somehow already connected server\n";
		}
	}

	/**
	 * @brief Handle read from client
	 */
	void Server::handle_read_event(const int& client_fd) {
		char buffer[READ_BUFFER];
		ssize_t bytesRead = recv(client_fd, buffer, 2048, 0);

		if (bytesRead == -1 || bytesRead == 0) {
			LOG_E() << "Failed to read data from client fd: " << client_fd << "\n";
			return remove_client(client_fd);
		}

		buffer[bytesRead] = '\0';
		LOG_I() << "Received a message from client fd: " << client_fd << ", message: " << buffer << "\n";

		if (_clients.count(client_fd) == 0) {
			LOG_E() << "Client fd: " << client_fd << " somehow not added into client list\n";
			return;
		}

		if (_clients.at(client_fd).get_method() == -1) {
			_clients.at(client_fd).init(std::string(buffer), _server_configs);
		}

		Request& req = _clients.at(client_fd);

		if (req.get_status_code() != 0 ||
			req.get_bytes_to_read() == 0) {
			_iohandler.set_write_ready(client_fd);
			return;
		} else if (req.get_bytes_to_read() > 0) {
			req.set_upload_file(buffer, bytesRead);
			req.update_bytes_to_read(bytesRead);
		}
	}

	/**
	 * @brief Handle write to client
	 */
	void Server::handle_write_event(const int& client_fd) {
		if (_clients.count(client_fd) == 0) {
			LOG_E() << "Client fd: " << client_fd << " somehow not added into client list\n";
			return;
		}

		Response response(_clients.at(client_fd));
		response.process();

		int ret = send(client_fd, response.get_raw_data().c_str(), response.get_raw_data().size(), 0);

		if (ret == -1 || ret == 0) {
			LOG_E() << "Failed to send the response to client fd: " << client_fd << "\n";
		} else {
			LOG_I() << "Send a response to client fd: " << client_fd << "\n";
		}

		remove_client(client_fd);
	}
} /* namespace webserv */

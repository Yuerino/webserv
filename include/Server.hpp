#pragma once

#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>

#include "ServerConfig.hpp"
#include "IOHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"

#ifndef READ_BUFFER
#define READ_BUFFER 2048
#endif

namespace webserv {
	namespace internal {
		extern bool g_shutdown;
	} /* namespace internal */
	class Server {
	public:
		Server(const std::vector<ServerConfig>& server_configs);
		~Server();

		void init();
		void run();
	private:
		std::vector<ServerConfig>	_server_configs;
		internal::IOHandler			_iohandler;
		std::set<Listen>			_listens;
		std::map<int, Listen>		_socket_fds;
		std::map<int, Request>		_clients;

		std::map<int, Listen>::iterator	socket_it;

		static int create_socket();
		static void bind_socket(const int& socket_fd, const std::string& host, const int& port);

		void remove_client(const int& client_fd);
		void handle_fail_event(const int& triggered_fd);
		void handle_accept_client(const int& socket_fd);
		void handle_read_event(const int& client_fd);
		void handle_write_event(const int& client_fd);

		Server(const Server& copy); /* disabled */
		Server& operator=(const Server& other); /* disabled */
	};
} /* namespace webserv */

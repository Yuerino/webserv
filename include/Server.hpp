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

namespace webserv {
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
		std::map<int, Request*>		_clients;

		std::map<int, Listen>::iterator	socket_it;

		Server(const Server& copy); /* disabled */
		Server& operator=(const Server& other); /* disabled */
	};
} /* namespace webserv */

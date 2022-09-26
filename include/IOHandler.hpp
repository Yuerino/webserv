#pragma once

#include <string>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>

#include "utils.hpp"

#ifdef __APPLE__
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#elif __linux__
#include <sys/epoll.h>
#endif

namespace webserv {
	namespace internal {
		class IOHandler {
		public:
			IOHandler();
			~IOHandler();

			int wait_for_new_event();
			int get_triggered_fd(const int& i);
			bool is_error(const int& i);
			bool is_eof(const int& i);
			bool is_read_ready(const int& i);
			bool is_write_ready(const int& i);
			void add_fd(const int& fd);
			void remove_fd(const int& fd);
			void set_write_ready(const int& fd);

			/* Getters */
			const int& get_poll_fd() const;
		private:
			int 				_poll_fd;

#ifdef __APPLE__
			struct kevent		_event_list[100];
#elif __linux__
			struct epoll_event	_event_list[100];
#endif

			IOHandler(const IOHandler& copy); /* disabled */
			IOHandler& operator=(const IOHandler& other); /* disabled */
		};
	} /* namespace internal */
} /* namespace webserv */

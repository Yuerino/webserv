#include "IOHandler.hpp"

#ifdef __APPLE__

namespace webserv {
	namespace internal {
		IOHandler::IOHandler() : _poll_fd(kqueue()) {}

		IOHandler::~IOHandler() {
			if (_poll_fd > 0) {
				close(_poll_fd);
			}
		}

		int IOHandler::wait_for_new_event() {
			int new_event_size = kevent(_poll_fd, NULL, 0, _event_list, 100, NULL);
			if (new_event_size == -1 && !g_shutdown) {
				throw std::runtime_error("Poll event failed: " + std::string(std::strerror(errno)) + "\n");
			}

			return new_event_size;
		}

		int IOHandler::get_triggered_fd(const int& i) {
			return _event_list[i].ident;
		}

		bool IOHandler::is_error(const int& i) {
			return _event_list[i].flags & EV_ERROR;
		}

		bool IOHandler::is_eof(const int& i) {
			return _event_list[i].flags & EV_EOF;
		}

		bool IOHandler::is_read_ready(const int& i) {
			return _event_list[i].filter == EVFILT_READ;
		}

		bool IOHandler::is_write_ready(const int& i) {
			return _event_list[i].filter == EVFILT_WRITE;
		}

		void IOHandler::add_fd(const int& fd) {
			struct kevent new_change;
			bzero(&new_change, sizeof(new_change));

			EV_SET(&new_change, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
			if (kevent(_poll_fd, &new_change, 1, NULL, 0, NULL) == -1) {
				throw std::runtime_error("Failed to add fd to poll: " + std::string(std::strerror(errno)) + "\n");
			}
		}

		void IOHandler::remove_fd(const int& fd) {
			struct kevent new_change;
			bzero(&new_change, sizeof(new_change));

			EV_SET(&new_change, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
			if (kevent(_poll_fd, &new_change, 1, NULL, 0, NULL) == -1) {
				throw std::runtime_error("Failed to remove fd from poll: " + std::string(std::strerror(errno)) + "\n");
			}
		}

		void IOHandler::set_write_ready(const int& fd) {
			struct kevent new_change;
			bzero(&new_change, sizeof(new_change));

			EV_SET(&new_change, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
			if (kevent(_poll_fd, &new_change, 1, NULL, 0, NULL) == -1) {
				throw std::runtime_error("Failed to set fd to write ready to poll: " + std::string(std::strerror(errno)) + "\n");
			}
		}

		/* Getters */
		const int& IOHandler::get_poll_fd() const { return _poll_fd; }
	} /* namespace internal */
} /* namespace webserv */

#elif __linux__

namespace webserv {
	namespace internal {
		IOHandler::IOHandler() : _poll_fd(epoll_create(69)) {}

		IOHandler::~IOHandler() {
			if (_poll_fd > 0) {
				close(_poll_fd);
			}
		}

		int IOHandler::wait_for_new_event() {
			int new_event_size = epoll_wait(_poll_fd, _event_list, 100, -1);
			if (new_event_size == 0) {
				LOG_D() << "No new event within timeout\n";
			} else if (new_event_size == -1 && !g_shutdown) {
				throw std::runtime_error("Poll event failed: " + std::string(std::strerror(errno)) + "\n");
			}

			return new_event_size;
		}

		int IOHandler::get_triggered_fd(const int& i) {
			return _event_list[i].data.fd;
		}

		bool IOHandler::is_error(const int& i) {
			return (_event_list[i].events & EPOLLERR) == EPOLLERR;
		}

		bool IOHandler::is_eof(const int& i) {
			return (_event_list[i].events & EPOLLHUP) == EPOLLHUP
				|| (_event_list[i].events & EPOLLRDHUP) == EPOLLRDHUP;
		}

		bool IOHandler::is_read_ready(const int& i) {
			return (_event_list[i].events & EPOLLIN) == EPOLLIN;
		}

		bool IOHandler::is_write_ready(const int& i) {
			return (_event_list[i].events & EPOLLOUT) == EPOLLOUT;
		}

		void IOHandler::add_fd(const int& fd) {
			struct epoll_event new_change;
			bzero(&new_change, sizeof(new_change));

			new_change.events = EPOLLIN | EPOLLPRI;
			new_change.data.fd = fd;
			if (epoll_ctl(_poll_fd, EPOLL_CTL_ADD, fd, &new_change) == -1) {
				throw std::runtime_error("Failed to add fd to poll: " + std::string(std::strerror(errno)) + "\n");
			}
		}

		void IOHandler::remove_fd(const int& fd) {
			if (epoll_ctl(_poll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
				throw std::runtime_error("Failed to remove fd from poll: " + std::string(std::strerror(errno)) + "\n");
			}
		}

		void IOHandler::set_write_ready(const int& fd) {
			struct epoll_event new_change;
			bzero(&new_change, sizeof(new_change));

			new_change.events = EPOLLOUT | EPOLLIN | EPOLLPRI;
			new_change.data.fd = fd;
			if (epoll_ctl(_poll_fd, EPOLL_CTL_MOD, fd, &new_change) == -1) {
				throw std::runtime_error("Failed to set fd to write ready to poll: " + std::string(std::strerror(errno)) + "\n");
			}
		}

		/* Getters */
		const int& IOHandler::get_poll_fd() const { return _poll_fd; }
	} /* namespace internal */
} /* namespace webserv */

#endif

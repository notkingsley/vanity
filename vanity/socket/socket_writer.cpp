//
// Created by kingsli on 9/16/23.
//

#include "socket.h"
#include "socket_server.h"
#include "socket_writer.h"


namespace vanity {

SocketWriter::SocketWriter(const ClientSocket &socket)
	: m_socket{socket} { }

SocketWriter::SocketWriter(SocketWriter &&other) noexcept
	: m_socket{other.m_socket}, m_responses{std::move(other.m_responses)} {}

SocketWriter::SocketWriter(const SocketWriter &other) : m_socket{other.m_socket} {
	m_responses = other.m_responses;
}

void SocketWriter::ready(SocketServer &server) {
	while (!m_responses.empty()) {
		auto& response = m_responses.front();
		if (response.write(m_socket))
			return;
		m_responses.pop();
	}
	server.remove_socket_writer(*this);
}

void SocketWriter::register_event(int epoll_fd) {
	m_socket.register_event(epoll_fd, *this);
}

void SocketWriter::unregister_event(int epoll_fd) const {
	m_socket.unregister_event(epoll_fd);
}

void SocketWriter::register_write(SocketServer &server, Response&& response) {
	m_responses.emplace(std::move(response));
	if (m_responses.size() == 1)
		server.add_socket_writer(*this);
}

} // namespace vanity

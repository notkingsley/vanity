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
	: m_socket{other.m_socket}, m_messages{std::move(other.m_messages)} {}

SocketWriter::SocketWriter(const SocketWriter &other) : m_socket{other.m_socket} {
	m_messages = other.m_messages;
}

void SocketWriter::ready(SocketServer &server) {
	while (!m_messages.empty()) {
		if (write())
			return;
		m_messages.pop();
		m_index = 0;
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
	m_messages.emplace(std::move(response.extract_data()));
	if (m_messages.size() == 1)
		server.add_socket_writer(*this);
}

bool SocketWriter::write(){
	try{
		auto& front = m_messages.front();
		m_index += m_socket.write(front.c_str() + m_index, front.size() - m_index);
		if (m_index == front.size())
			return false;
		return true;
	}
	catch (SocketError& e)
	{
		if (e.get_errno() == EAGAIN)
			return true;
		else
			throw;
	}
}

} // namespace vanity

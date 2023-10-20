//
// Created by kingsli on 9/16/23.
//

#include "socket.h"
#include "socket_server.h"
#include "socket_writer.h"


namespace vanity {

MessageWriter::MessageWriter(const std::string &msg) {
	m_message.reserve(msg.size() + 10);
	for (char c : msg){
		m_message.push_back(c);
		if (c == m_delimiter)
			m_message.push_back(m_delimiter);
	}
	m_message.push_back(m_delimiter);

	// ignored, but ensures delimiter is not the last char
	m_message.push_back('\n');
}

bool MessageWriter::write(const ClientSocket &socket) {
	try{
		m_index += socket.write(
		m_message.c_str() + m_index,
		m_message.size() - m_index
		);
		if (m_index == m_message.size())
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

SocketWriter::SocketWriter(const ClientSocket &socket)
	: m_socket{socket} { }

SocketWriter::SocketWriter(SocketWriter &&other) noexcept
	: m_socket{other.m_socket}, m_message_writers{std::move(other.m_message_writers)} {}

SocketWriter::SocketWriter(const SocketWriter &other) : m_socket{other.m_socket} {
	m_message_writers = other.m_message_writers;
}

void SocketWriter::ready(SocketServer &server) {
	while (!m_message_writers.empty()) {
		auto& writer = m_message_writers.front();
		if (writer.write(m_socket))
			return;
		m_message_writers.pop();
	}
	server.remove_socket_writer(*this);
}

void SocketWriter::register_event(int epoll_fd) {
	m_socket.register_event(epoll_fd, *this);
}

void SocketWriter::unregister_event(int epoll_fd) const {
	m_socket.unregister_event(epoll_fd);
}

void SocketWriter::register_write(SocketServer &server, const std::string &msg) {
	m_message_writers.emplace(msg);
	if (m_message_writers.size() == 1)
		server.add_socket_writer(*this);
}

} // namespace vanity

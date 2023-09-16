//
// Created by kingsli on 9/16/23.
//

#include "socket_writer.h"


namespace vanity {

SocketWriter::SocketWriter(const ClientSocket &socket, const std::string& msg)
	: m_socket{socket}
{
	m_message.reserve(msg.size() + 1);
	for (char c : msg){
		m_message.push_back(c);
		if (c == m_delimiter)
			m_message.push_back(m_delimiter);
	}
	m_message.push_back(m_delimiter);
}

bool SocketWriter::ready(AbstractServer &server) {
	try{
		m_index += m_socket.write(
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
	};

void SocketWriter::register_event(int epoll_fd) {
	m_socket.register_event(epoll_fd, *this);
}

void SocketWriter::unregister_event(int epoll_fd) {
	m_socket.unregister_event(epoll_fd);
}


} // namespace vanity

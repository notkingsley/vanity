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

int SocketWriter::socket_fd() const {
	return m_socket.fd();
}

void SocketWriter::ready(SocketServer &server) {
	while (!m_messages.empty()) {
		if (do_write())
			return;
		m_messages.pop();
		m_index = 0;
	}
	server.remove_socket_writer(*this);
}

void SocketWriter::write(SocketServer &server, std::string&& response) {
	m_messages.emplace(std::move(response));
	if (m_messages.size() == 1)
		server.add_socket_writer(*this);
}

bool SocketWriter::do_write(){
	try{
		auto& front = m_messages.front();
		m_index += m_socket.write(front.c_str() + m_index, front.size() - m_index);
		return m_index != front.size();
	}
	catch (SocketError& e)
	{
		if (e.is_retry())
			return true;
		throw;
	}
}

} // namespace vanity

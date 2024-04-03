//
// Created by kingsli on 9/16/23.
//

#include "exceptions.h"
#include "socket.h"
#include "socket_writer.h"


namespace vanity::socket {

SocketWriter::SocketWriter(const Socket &socket)
	: m_socket{socket} { }

SocketWriter::SocketWriter(SocketWriter &&other) noexcept
	: m_socket{other.m_socket}, m_messages{std::move(other.m_messages)} {}

SocketWriter::SocketWriter(const SocketWriter &other) : m_socket{other.m_socket} {
	m_messages = other.m_messages;
}

int SocketWriter::socket_fd() const {
	return m_socket.fd();
}

void SocketWriter::ready(WriteManager& manager) {
	std::lock_guard lock(m_mutex);
	try_write_all();

	if (m_messages.empty())
		manager.remove_writer(*this);
}

void SocketWriter::write(WriteManager& manager, std::string&& response) {
	std::lock_guard lock(m_mutex);
	auto was_empty = m_messages.empty();

	m_messages.emplace(std::move(response));
	try_write_all();

	if (was_empty and !m_messages.empty())
		manager.add_writer(*this);
}

void SocketWriter::try_write_all() {
	while (!m_messages.empty()) {
		if (do_write())
			return;
		m_messages.pop();
		m_index = 0;
	}
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

} // namespace vanity::socket

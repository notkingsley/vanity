//
// Created by kingsli on 9/16/23.
//

#include "exceptions.h"
#include "socket.h"
#include "socket_writer.h"


namespace vanity::socket {

SocketWriter::SocketWriter(const Socket &socket)
	: m_socket{socket} { }

int SocketWriter::socket_fd() const {
	return m_socket.fd();
}

void SocketWriter::ready(WriteManager& manager) {
	std::lock_guard lock(m_mutex);
	try_write_all();

	if (m_message.empty())
		manager.remove_writer(*this);
}

void SocketWriter::write(WriteManager& manager, std::string&& response) {
	std::lock_guard lock(m_mutex);
	auto was_empty = m_message.empty();

	if (was_empty)
		m_message = std::move(response);
	else
		m_message += response;

	try_write_all();
	if (was_empty and !m_message.empty())
		manager.add_writer(*this);
}

void SocketWriter::try_write_all() {
	while (m_index < m_message.size()) {
		if (do_write())
			return;

		m_index = 0;
		m_message.clear();
	}
}

bool SocketWriter::do_write(){
	try {
		m_index += m_socket.write(m_message.c_str() + m_index, m_message.size() - m_index);
		return m_index != m_message.size();
	}
	catch (SocketError& e) 	{
		if (e.is_retry())
			return true;

		throw;
	}
}

} // namespace vanity::socket

#include "socket_reader.h"

namespace vanity::socket {

bool SocketReader::read(Socket& socket, const callback_t& callback)
{
	if (m_message_left == 0) {
		uint32_t size = 0;
		if (socket.read(reinterpret_cast<char*>(&size), sizeof(size)) == 0)
			return false;

		m_message_left = ntohl(size);
		m_message.resize(m_message_left);
	}

	size_t bytes_read = socket.read(
		m_message.data() + m_message.size() - m_message_left,
		m_message_left
	);
	if (bytes_read == 0)
		return false;

	m_message_left -= bytes_read;
	if (m_message_left == 0) {
		callback(m_message);
		m_message.clear();
	}

	return true;
}

} // namespace vanity::socket

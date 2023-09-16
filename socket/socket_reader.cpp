#include "socket_reader.h"


namespace vanity{

SocketReader::SocketReader(ClientSocket&& socket)
	: m_socket(std::move(socket)) {}

bool SocketReader::ready(AbstractServer& server)
{
	char buffer[m_chunk_size];
	buffer[0] = m_delimiter;
	int offset = m_delimiter_read ? 1 : 0;

	size_t bytes_read = m_socket.read(
			buffer + offset,
			m_chunk_size - offset
	);
	if (bytes_read == 0)
		return false;

	bytes_read += offset;
	m_delimiter_read = false;

	for (size_t i = 0; i < bytes_read; ++i){
		if (buffer[i] == m_delimiter)
			if (i == bytes_read - 1){ // end of the buffer
				m_delimiter_read = true;
			} else {
				if (buffer[i + 1] == m_delimiter){ // escaped delimiter
					m_message += m_delimiter;
					++i;
				} else { // end of message
					server.handle(m_message, m_socket);
					m_message.clear();
				}
			}
		else
			m_message += buffer[i];
	}

	return true;
}

void SocketReader::register_event(int epoll_fd) {
	m_socket.register_event(epoll_fd, *this);
}

void SocketReader::unregister_event(int epoll_fd) {
	m_socket.unregister_event(epoll_fd);
}

} // namespace vanity
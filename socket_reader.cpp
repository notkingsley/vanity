#include "socket_reader.h"


namespace vanity{

SocketReader::SocketReader(ClientSocket& socket)
	: m_socket(socket) {}

bool SocketReader::read(AbstractBaseServer& server)
{
	char buffer[m_chunk_size];
	for (size_t i = 0; i < m_delimiter_read; ++i){
		buffer[i] = m_delimiter[i];
	}
	size_t bytes_read = m_socket.read(
			buffer + m_delimiter_read,
			m_chunk_size - m_delimiter_read
	);
	if (bytes_read == 0)
		return false;

	bytes_read += m_delimiter_read;
	m_delimiter_read = 0;

	size_t i = 0;
	for (; i < bytes_read; ++i){
		// search forward for the delimiter
		size_t j = 0;
		for (; j < m_delimiter.length() and i + j < bytes_read; ++j)
			if (buffer[i + j] != m_delimiter[j])
				break;

		if (j == m_delimiter.length()){
			// found the delimiter
			server.handle(m_message, m_socket);
			m_message.clear();
			i += m_delimiter.length() - 1;

		} else if (i + j == bytes_read){
			// found part of the delimiter, but buffer is exhausted
			m_delimiter_read = j;
			break;

		} else {
			m_message += buffer[i];
		}
	}
	return true;
}

} // namespace vanity
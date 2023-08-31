#include "socket_reader.h"


namespace vanity{

SocketReader::SocketReader(BaseServer& server, ClientSocket& socket)
	: m_server(server), m_socket(socket) {}

bool SocketReader::read()
{
	char buffer[m_chunk_size];
	for (size_t i = 0; i < m_delimiter_read; ++i){
		buffer[i] = m_delimiter[i];
	}
	size_t bytes_read = m_socket.read(
		buffer + m_delimiter_read,
		m_chunk_size - m_delimiter_read
	);

	if (bytes_read == 0){
		// socket is closed
		return false;
	}
	bytes_read += m_delimiter_read;
	m_delimiter_read = 0;

	for (size_t i = 0; i < bytes_read; ++i){
		if (this->ignores(buffer[i]))
			continue;

		// search forward for the delimiter
		// j is the number of characters read ahead
		// d is the number of characters read from the delimiter
		size_t j = 0, d = 0;
		for (; d < m_delimiter.length() and i + j < bytes_read; ++j, ++d){
			if (this->ignores(buffer[i + j])){
				--d;
				continue;
			}
			if (buffer[i + j] != m_delimiter[j]){
				break;
			}
		}

		if (d == m_delimiter.length()){
			// found the delimiter
			std::string msg;
			std::swap(msg, m_buffer);
			this->emit(msg);
			i += m_delimiter.length() - 1;
		} else if (i + j == bytes_read){
			// found part of the delimiter, but buffer is exhausted
			m_delimiter_read = d;
			break;
		} else {
			// didn't find the delimiter
			m_buffer.push_back(buffer[i]);
		}
	}
	return true;
}

} // namespace vanity
#include "socket_reader.h"


namespace vanity{

SocketReader::SocketReader(ClientSocket& socket)
	: m_socket(socket) {}

std::pair<bool, std::string> SocketReader::read()
{
	char* buffer = nullptr;
	char stack_buffer[m_chunk_size];
	if (m_buffer.empty()){
		buffer = stack_buffer;
	} else {
		m_buffer.reserve(m_chunk_size);
		buffer = m_buffer.data();
	}

	size_t buflen = m_buffer.length();
	size_t bytes_read = m_socket.read(
		buffer + buflen,
		m_chunk_size - buflen
	);
	bytes_read += buflen;
	if (bytes_read == 0){
		return {false, {}};
	}

	bool found = false;
	size_t i = 0;
	for (; i < bytes_read; ++i){
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
			found = true;
			size_t k = i + m_delimiter.length();
			m_buffer = std::string{&buffer[k], bytes_read - k};
			break;
		} else if (i + j == bytes_read){
			// found part of the delimiter, but buffer is exhausted
			m_buffer = std::string{m_delimiter, 0, d};
			break;
		}
	}
	m_message.append(buffer, i);

	std::string msg;
	if (found)
		std::swap(msg, m_message);

	if (i == bytes_read)
		// exhausted buffer, clear in case m_buffer was buffer
		m_buffer.clear();
	
	return {true, msg};
}

} // namespace vanity
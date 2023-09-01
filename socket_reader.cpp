#include "socket_reader.h"


namespace vanity{

SocketReader::SocketReader(ClientSocket& socket)
	: m_socket(socket) {}

std::pair<bool, std::string> SocketReader::read()
{
	std::string msg;
	char buffer[m_chunk_size];
	for (size_t i = 0; i < m_buffer.length(); ++i){
		buffer[i] = m_buffer[i];
	}
	size_t bytes_read = m_socket.read(
		buffer + m_buffer.length(),
		m_chunk_size - m_buffer.length()
	);

	if (bytes_read == 0){
		return {false, msg};
	}
	bytes_read += m_buffer.length();
	m_buffer.clear();

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
			std::swap(msg, m_message);
			size_t k = i + m_delimiter.length();
			m_buffer = std::string{&buffer[k], bytes_read - k};
			break;
		} else if (i + j == bytes_read){
			// found part of the delimiter, but buffer is exhausted
			m_buffer = std::string{m_delimiter, 0, d};
			break;
		} else {
			// didn't find the delimiter
			m_message.push_back(buffer[i]);
		}
	}
	return {true, msg};
}

} // namespace vanity
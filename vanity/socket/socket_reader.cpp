#include "client.h"
#include "socket_reader.h"


namespace vanity{

bool SocketReader::read(AbstractServer& server, const Client& client)
{
	char buffer[m_chunk_size];
	buffer[0] = m_delimiter;
	int offset = m_delimiter_read ? 1 : 0;

	size_t bytes_read = client.socket().read(
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
					server.handle(m_message, client.socket());
					m_message.clear();
				}
			}
		else
			m_message += buffer[i];
	}

	return true;
}

} // namespace vanity
#include "abstract_server.h"
#include "socket_reader.h"


namespace vanity{

bool SocketReader::read(AbstractServer& server, Client& client)
{
	if (m_message_left == 0) {
		uint32_t size = 0;
		if (client.read((char*)&size, sizeof(size)) == 0)
			return false;
		m_message_left = ntohl(size);
		m_message.resize(m_message_left);
	}

	size_t bytes_read = client.read(
		m_message.data() + m_message.size() - m_message_left,
		m_message_left
	);
	if (bytes_read == 0)
		return false;

	m_message_left -= bytes_read;
	if (m_message_left == 0){
		server.handle(m_message, client);
		m_message.clear();
	}

	return true;
}

} // namespace vanity
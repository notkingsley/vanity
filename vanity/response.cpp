//
// Created by kingsli on 10/26/23.
//

#include <cstring>
#include "response.h"


namespace vanity {

Response::Response(const std::string &data) {
	m_data.resize(M_LENGTH_SIZE);
	m_data += data;
}

bool Response::write(const ClientSocket &socket) {
	if (m_index == 0) {
		auto size = htonl(m_data.size() - M_LENGTH_SIZE);
		std::memcpy(m_data.data(), &size, M_LENGTH_SIZE);
	}
	try{
		m_index += socket.write(
			m_data.c_str() + m_index,
			m_data.size() - m_index
		);
		if (m_index == m_data.size())
			return false;
		return true;
	}
	catch (SocketError& e)
	{
		if (e.get_errno() == EAGAIN)
			return true;
		else
			throw;
	}
}

Response &Response::operator<<(const std::string &data) {
	m_data += data;
	return *this;
}

Response &Response::operator<<(const char *data) {
	m_data += data;
	return *this;
}

} // namespace vanity
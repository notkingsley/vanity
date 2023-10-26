//
// Created by kingsli on 10/26/23.
//

#include <cstring>
#include "response.h"


namespace vanity {

Response::Response() {
	m_data.resize(M_LENGTH_SIZE);
}

Response::Response(const std::string &data) : Response() {
	m_data.resize(M_LENGTH_SIZE);
	m_data += data;
}

std::string&& Response::extract_data() {
	auto size = htonl(m_data.size() - M_LENGTH_SIZE);
	std::memcpy(m_data.data(), &size, M_LENGTH_SIZE);
	return std::move(m_data);
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
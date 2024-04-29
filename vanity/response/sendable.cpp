//
// Created by kingsli on 4/29/24.
//

#include <cstring>

#include "sendable.h"

namespace vanity {

Sendable::Sendable() {
	m_data.resize(M_LENGTH_SIZE);
}

std::string&& Sendable::extract_data() {
	auto size = htonl(m_data.size() - M_LENGTH_SIZE);
	std::memcpy(m_data.data(), &size, M_LENGTH_SIZE);
	return std::move(m_data);
}

std::string_view Sendable::body() const {
	return {m_data.data() + M_LENGTH_SIZE, m_data.size() - M_LENGTH_SIZE};
}

void Sendable::reserve(size_t size) {
	m_data.reserve(size + m_data.size());
}

Sendable &&Sendable::move() {
	return std::move(*this);
}

Sendable &Sendable::operator<<(const std::string &data) {
	reserve(data.size());
	m_data += data;
	return *this;
}

Sendable &Sendable::operator<<(const char *data) {
	m_data += data;
	return *this;
}

Sendable &Sendable::operator<<(const char data) {
	m_data += data;
	return *this;
}

} // namespace vanity

//
// Created by kingsli on 12/21/23.
//

#include "fake_client.h"

namespace vanity {

FakeClient::FakeClient(Client &client)  : m_client(client) {}

void FakeClient::ready(SocketServer &server) {
	return m_client.ready(server);
}

bool FakeClient::has_perm(operation_t op) const {
	return m_client.has_perm(op);
}

struct session_info &FakeClient::session_info() {
	return m_client.session_info();
}

void FakeClient::close() {
	m_client.close();
}

size_t FakeClient::read(char *buffer, size_t buffer_size) const {
	return m_client.read(buffer, buffer_size);
}

void FakeClient::write(SocketServer &server, Response &&response) {
	m_client.write(server, std::move(response));
}

Client &FakeClient::client() const {
	return m_client;
}

} // namespace vanity
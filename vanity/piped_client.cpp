//
// Created by kingsli on 11/12/23.
//

#include "piped_client.h"

namespace vanity {

void PipedClient::ready(SocketServer &server) {
	return m_client.ready(server);
}

bool PipedClient::has_perm(operation_t op) const {
	return m_client.has_perm(op);
}

void PipedClient::db(int db) {
	return m_client.db(db);
}

int PipedClient::db() const {
	return m_client.db();
}

void PipedClient::set_auth(const client_auth &auth) {
	return m_client.set_auth(auth);
}

void PipedClient::username(const std::string &username) {
	return m_client.username(username);
}

const std::string &PipedClient::username() const {
	return m_client.username();
}

void PipedClient::close() {
	m_client.close();
}

size_t PipedClient::read(char *buffer, size_t buffer_size) const {
	return m_client.read(buffer, buffer_size);
}

void PipedClient::write(SocketServer &server, Response &&response) {
	m_response.append(response);
}

void PipedClient::perform_write(SocketServer &server) {
	m_client.write(server, m_response.move());
}

} // namespace vanity
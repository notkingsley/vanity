//
// Created by kingsli on 10/17/23.
//

#include "client.h"
#include "socket/socket_server.h"

namespace vanity {

Client::Client(ClientSocket&& socket) : SocketClient(std::move(socket)) {};

void Client::ready(SocketServer &server) {
	try{
		if(m_closed or not m_reader.read(server, *this))
			// Warning: this will delete this object
			server.remove_client(*this);
	}
	catch (DestroyClient& e) {
		// Warning: this will delete this object
		server.remove_client(*this);
	}
}

bool Client::has_perm(operation_t op) const {
	return is_permitted(op, m_session_info.auth);
}

void Client::set_db(int db) const {
	m_session_info.database = db;
}

void Client::set_auth(const client_auth &auth) const {
	m_session_info.auth = auth;
}

void Client::username(const std::string &username) const {
	m_session_info.username = username;
}

const std::string &Client::username() const {
	return m_session_info.username;
}

void Client::close() const {
	m_closed = true;
}

bool operator==(const Client& lhs, const Client& rhs) {
	return &lhs == &rhs;
}

} // namespace vanity
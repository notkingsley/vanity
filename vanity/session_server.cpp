//
// Created by kingsli on 10/9/23.
//

#include "session_server.h"

namespace vanity {

client_auth SessionServer::session_auth(Client &client) {
	return client.session_info().auth;
}

void SessionServer::session_set_auth(Client &client, client_auth auth) {
	auto &session_info = client.session_info();
	switch (auth) {
		case client_auth::USER:
		case client_auth::ADMIN: {
			break;
		}
		case client_auth::PEER: {
			throw std::runtime_error("address required to set auth to PEER");
		}
		case client_auth::UNKNOWN: {
			throw std::runtime_error("cannot set auth to UNKNOWN");
		}
	}

	session_info.auth = auth;
	session_info.user_data = std::make_unique<user_data_t>();
}

void SessionServer::session_set_auth(Client &client, client_auth auth, const std::string& addr, std::optional<std::string> id) {
	if (auth != client_auth::PEER)
		throw std::runtime_error("can only set to PEER with an address");

	auto &session_info = client.session_info();
	session_info.auth = auth;
	session_info.peer_data = std::make_unique<peer_data_t>(addr, std::move(id));

}


bool SessionServer::session_is_peer(Client &client) {
	return session_auth(client) == client_auth::PEER;
}

bool SessionServer::session_is_user(Client &client) {
	auto auth = session_auth(client);
	return auth == client_auth::USER or auth == client_auth::ADMIN;
}

user_data_t& SessionServer::session_user_data(Client& client) {
	if (auto &user_data = client.session_info().user_data)
		return *user_data;

	throw std::runtime_error("client is not a user");
}


uint& SessionServer::session_db(Client &client) {
	return session_user_data(client).database;
}

std::string &SessionServer::session_username(Client &client) {
	return session_user_data(client).username;
}

user_data_t::channels_t &SessionServer::session_channels(Client &client) {
	return session_user_data(client).channels;
}

uint64_t &SessionServer::session_trn_id(Client &client) {
	return session_user_data(client).trn_id;
}

peer_data_t &SessionServer::session_peer_data(Client &client) {
	if (auto &peer_data = client.session_info().peer_data)
		return *peer_data;

	throw std::runtime_error("client is not a peer");
}

std::string &SessionServer::session_addr(Client &client) {
	return session_peer_data(client).addr;
}

std::optional<std::string> &SessionServer::session_id(Client &client) {
	return session_peer_data(client).id;
}

} // namespace vanity

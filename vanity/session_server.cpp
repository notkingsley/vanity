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
			session_info.auth = auth;
			session_info.user_data = std::make_unique<user_data_t>();
			break;
		}
		case client_auth::PEER: {
			throw std::runtime_error("address required to set auth to PEER");
		}
		case client_auth::UNKNOWN: {
			throw std::runtime_error("cannot set auth to UNKNOWN");
		}
	}
}

void SessionServer::session_set_auth(Client &client, client_auth auth, const std::string& addr) {
	if (auth != client_auth::PEER)
		throw std::runtime_error("can only set to PEER with an address");

	auto &session_info = client.session_info();
	session_info.auth = auth;
	session_info.peer_data = std::make_unique<peer_data_t>(addr);

}


bool SessionServer::session_is_peer(Client &client) {
	return session_auth(client) == client_auth::PEER;
}

bool SessionServer::session_is_user(Client &client) {
	auto auth = session_auth(client);
	return auth == client_auth::USER or auth == client_auth::ADMIN;
}

uint& SessionServer::session_db(Client &client) {
	if (auto &user_data = client.session_info().user_data)
		return user_data->database;

	throw std::runtime_error("client is not a user");
}

std::string &SessionServer::session_username(Client &client) {
	if (auto &user_data = client.session_info().user_data)
		return user_data->username;

	throw std::runtime_error("client is not a user");
}

user_data_t::conn_state SessionServer::session_state(Client &client) {
	auto &session_info = client.session_info();
	if (not session_info.user_data and not session_info.peer_data)
		return user_data_t::conn_state::NORMAL;

	if (session_info.user_data)
		return session_info.user_data->state;

	throw std::runtime_error("client is not a user");
}

void SessionServer::session_set_state(Client &client, user_data_t::conn_state state) {
	auto &user_data = client.session_info().user_data;
	if (not user_data)
		throw std::runtime_error("client is not a user");

	user_data->state = state;
	switch (state) {
		case user_data_t::conn_state::NORMAL: {
			user_data->trn_data.reset();
			break;
		}
		case user_data_t::conn_state::TRANSACTION: {
			user_data->trn_data = std::make_unique<user_data_t::transaction_data_t>();
			break;
		}
	}
}

user_data_t::channels_t &SessionServer::session_channels(Client &client) {
	if (auto &user_data = client.session_info().user_data)
		return user_data->channels;

	throw std::runtime_error("client is not a user");
}

user_data_t::transaction_data_t &SessionServer::session_transaction_data(Client &client) {
	auto &user_data = client.session_info().user_data;
	if (not user_data)
		throw std::runtime_error("client is not a user");

	if (auto &conn_data = user_data->trn_data)
		return *conn_data;

	throw std::runtime_error("client is not in a transaction");
}

peer_data_t &SessionServer::session_peer_data(Client &client) {
	if (auto &peer_data = client.session_info().peer_data)
		return *peer_data;

	throw std::runtime_error("client is not a peer");
}

std::string &SessionServer::session_addr(Client &client) {
	if (auto &peer_data = client.session_info().peer_data)
		return peer_data->addr;

	throw std::runtime_error("client is not a peer");
}

} // namespace vanity

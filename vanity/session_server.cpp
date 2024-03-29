//
// Created by kingsli on 10/9/23.
//

#include "session_server.h"

namespace vanity {

uint& SessionServer::session_db(Client &client) {
	return client.session_info().database;
}

client_auth &SessionServer::session_auth(Client &client) {
	return client.session_info().auth;
}

std::string &SessionServer::session_username(Client &client) {
	return client.session_info().username;
}

conn_state SessionServer::session_state(Client &client) {
	return client.session_info().state;
}

void SessionServer::session_set_state(Client &client, conn_state state) {
	auto &session_info = client.session_info();
	session_info.state = state;

	switch (state) {
		case conn_state::NORMAL: {
			session_info.conn_data.reset();
			break;
		}
		case conn_state::TRANSACTION: {
			session_info.conn_data = std::make_unique<transaction_data>();
			break;
		}
	}
}

session_info::channels_t &SessionServer::session_channels(Client &client) {
	return client.session_info().channels;
}

} // namespace vanity
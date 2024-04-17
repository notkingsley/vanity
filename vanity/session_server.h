//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_SESSION_SERVER_H
#define VANITY_SESSION_SERVER_H

#include "client/client.h"

namespace vanity {

/*
 * A SessionServer manages a client's session_info
 *
 * The purpose of this class is to ensure invariants on session_info
 */
class SessionServer
{
public:
	// get a reference to a client's current db
	static uint& session_db(Client& client);

	// get a reference to a client's current auth level
	static client_auth& session_auth(Client& client);

	// get a reference to a client's username
	static std::string& session_username(Client& client);

	// get a client's current connection state
	static conn_state session_state(Client& client);

	// set a client's current connection state
	static void session_set_state(Client& client, conn_state state);

	// get a client's current channels
	static session_info::channels_t &session_channels(Client &client);

	// get a reference to a client's current connection's state data
	template<conn_state state>
	static conn_data_t<state>& session_data(Client& client) {
		auto &session_info = client.session_info();
		if (session_info.state != state)
			throw std::runtime_error("client is not in the correct state");

		if (not session_info.conn_data)
			throw std::runtime_error("state does not hold any data");

		return *session_info.conn_data;
	}

	// check if a client is a peer
	static bool session_is_peer(Client& client);
};

} // namespace vanity

#endif //VANITY_SESSION_SERVER_H

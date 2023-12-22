//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_SESSION_SERVER_H
#define VANITY_SESSION_SERVER_H

#include "client.h"

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
	static int& session_db(Client& client);

	// get a reference to a client's current auth level
	static client_auth& session_auth(Client& client);

	// get a reference to a client's username
	static std::string& session_username(Client& client);

	// get a client's current connection state
	static conn_state session_state(Client& client);

	// set a client's current connection state
	static void session_set_state(Client& client, conn_state state);

	// get a reference to a client's current connection's state data
	template<conn_state state>
	static conn_data_t<state>& session_data(Client& client) {
		auto &session_info = client.session_info();
		if (session_info.state != state)
			throw std::runtime_error("client is not in the correct state");

		if (not session_info.conn_data)
			throw std::runtime_error("state does not hold any data");

		if (not std::holds_alternative<conn_data_t<state>>(session_info.conn_data))
			throw std::runtime_error("variant data is of wrong type");

		return std::get<conn_data_t<state>>(*session_info.conn_data);
	}
};

} // namespace vanity

#endif //VANITY_SESSION_SERVER_H

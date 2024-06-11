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
	// get a client's current auth level
	static client_auth session_auth(Client& client);

	// set a client's auth level
	static void session_set_auth(Client& client, client_auth auth);

	// set a client's auth level for a peer
	static void session_set_auth(Client& client, client_auth auth, const std::string& addr);

	// check if a client is a peer
	static bool session_is_peer(Client& client);

	// check if a client is a user
	static bool session_is_user(Client& client);

	// get a reference to a client's current db
	static uint& session_db(Client& client);

	// get a reference to a client's username
	static std::string& session_username(Client& client);

	// get a client's current connection state
	static user_data_t::conn_state session_state(Client& client);

	// set a client's current connection state
	static void session_set_state(Client& client, user_data_t::conn_state state);

	// get a client's current channels
	static user_data_t::channels_t &session_channels(Client &client);

	// get a reference to a client's current transaction data
	static user_data_t::transaction_data_t& session_transaction_data(Client& client);

	// get a client's current peer data
	static peer_data_t& session_peer_data(Client& client);

	// get a peer's address
	static std::string& session_addr(Client& client);
};

} // namespace vanity

#endif //VANITY_SESSION_SERVER_H

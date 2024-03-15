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

protected:
	/*
	 * Temporarily changes a client's state
	 */
	class TempState
	{
	private:
		// the client
		Client& m_client;

		// the client's original state
		conn_state m_original_state;

		// get a reference to the client's state
		conn_state& state() const {
			return m_client.session_info().state;
		}

	public:
		// create a TempState with a client and a new state
		TempState(Client& client, conn_state new_state)
				: m_client(client), m_original_state(state()) {
			state() = new_state;
		}

		// restore the client's original state
		~TempState() {
			state() = m_original_state;
		}

		// delete copy constructor
		TempState(const TempState&) = delete;

		// delete copy assignment
		TempState& operator=(const TempState&) = delete;

		// delete move constructor
		TempState(TempState&&) = delete;

		// delete move assignment
		TempState& operator=(TempState&&) = delete;
	};
};

} // namespace vanity

#endif //VANITY_SESSION_SERVER_H

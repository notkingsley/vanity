//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_CLIENT_H
#define VANITY_CLIENT_H

#include "permissions.h"
#include "socket/socket_client.h"

namespace vanity {

/*
 * A Client is, well, a client
 */
class Client : public SocketClient
{
private:
	// the client's session info
	mutable session_info m_session_info;

public:
	// create a client
	explicit Client(ClientSocket&& socket) : SocketClient(std::move(socket)) {};

	// the client has sent a message, and it is ready to be read
	bool ready(AbstractServer& server) override
	{
		return m_reader.read(server, *this);
	}

	// check if the client has permission to perform an op
	bool has_perm(operation_t op) const
	{
		return is_permitted(op, m_session_info.auth);
	}

	// set the client's active database index
	void set_db(int db) const
	{
		m_session_info.database = db;
	}

	// update the client's auth level
	void set_auth(const client_auth& auth) const
	{
		m_session_info.auth = auth;
	}

	// set the client's username
	void username(const std::string& username) const
	{
		m_session_info.username = username;
	}

	// get the client's username
	const std::string& username() const
	{
		return m_session_info.username;
	}
};

} // namespace vanity

#endif //VANITY_CLIENT_H

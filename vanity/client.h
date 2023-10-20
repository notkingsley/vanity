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

	// if this client has been requested to be closed
	mutable bool m_closed = false;

public:
	// create a client
	explicit Client(ClientSocket&& socket);

	// the client has sent a message, and it is ready to be read
	void ready(SocketServer& server) override;

	// check if the client has permission to perform an op
	bool has_perm(operation_t op) const;

	// set the client's active database index
	void db(int db);

	// get the client's active database index
	int db() const;

	// update the client's auth level
	void set_auth(const client_auth& auth);

	// set the client's username
	void username(const std::string& username);

	// get the client's username
	const std::string& username() const;

	// request to close the client, not effective immediately
	// but when the client makes a request
	void close();
};

bool operator==(const Client& lhs, const Client& rhs);

} // namespace vanity

namespace std {

template<>
struct hash<vanity::Client> : public hash<const vanity::Client*> {
	size_t operator()(const vanity::Client &c) const {
		return hash<const vanity::Client*>::operator()(&c);
	}
};

} // namespace std

#endif // VANITY_CLIENT_H

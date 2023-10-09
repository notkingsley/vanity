//
// Created by kingsli on 10/9/23.
//

#ifndef VANITY_CLIENT_H
#define VANITY_CLIENT_H

#include "socket/socket_client.h"

namespace vanity {

/*
 * A Client is, well, a client
 */
class Client : public SocketClient
{
public:
	// create a client
	explicit Client(ClientSocket&& socket) : SocketClient(std::move(socket)) {};

	// the client has sent a message, and it is ready to be read
	bool ready(AbstractServer& server) override
	{
		return m_reader.read(server, *this);
	}
};

} // namespace vanity

#endif //VANITY_CLIENT_H

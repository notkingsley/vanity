//
// Created by kingsli on 5/4/24.
//

#ifndef VANITY_EXIT_SERVER_H
#define VANITY_EXIT_SERVER_H

#include "request/request_server.h"
#include "tcp_client.h"

namespace vanity {

/*
 * An ExitServer is a server that handles closing clients
 */
class ExitServer:
	public virtual RequestServer
{
protected:
	// close a client
	// throws if the client is not a TcpClient (like a ProxyClient)
	static void client_close(Client& client) {
		if (auto tcp_client = dynamic_cast<TcpClient*>(&client))
			tcp_client->close();
		else
			throw std::runtime_error("request_exit: client is not a TcpClient");
	}

public:
	// an exit request was received from a client
	void request_exit(Client& client) override {
		client_close(client);
	};

	// a terminate request was received from a client
	void request_terminate(Client& client) override {
		terminate();
	};
};

} // namespace vanity

#endif //VANITY_EXIT_SERVER_H

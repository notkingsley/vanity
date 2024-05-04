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
		to_tcp(client).close();
	}

	// cast a client to a TcpClient or throw an exception
	static TcpClient& to_tcp(Client& client) {
		if (auto tcp = dynamic_cast<TcpClient*>(&client))
			return *tcp;

		throw std::runtime_error("expected a TcpClient");
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

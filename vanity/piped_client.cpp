//
// Created by kingsli on 11/12/23.
//

#include "piped_client.h"

namespace vanity {

PipedClient::PipedClient(Client &client, size_t pipe_size) : FakeClient(client), m_response(pipe_size) {}

void PipedClient::write(SocketServer &server, Response &&response) {
	m_response.append(response);
}

void PipedClient::perform_write(SocketServer &server) {
	client().write(server, m_response.move());
}

} // namespace vanity
//
// Created by kingsli on 11/15/23.
//

#include "piped_client.h"
#include "pipe_server.h"

namespace vanity {

// extract a (len) from part of a message (defined by request_server)
extern size_t extract_len(const std::string& msg, size_t& pos);

void PipeServer::request_pipe(Client &client, const std::string &msg, size_t &pos) {
	size_t len = extract_len(msg, pos);
	PipedClient piped_client{client, len};

	for (size_t i = 0; i < len - 1; ++i)
		do_handle(piped_client, msg, pos, false);
	do_handle(piped_client, msg, pos, true);

	piped_client.perform_write(*this);
}

} // namespace vanity
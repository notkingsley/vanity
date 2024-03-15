//
// Created by kingsli on 2/12/24.
//

#ifndef VANITY_WAL_RECOVERY_CLIENT_H
#define VANITY_WAL_RECOVERY_CLIENT_H

#include "client/client.h"

namespace vanity::wal {

class WalRecoveryClient : public Client
{
private:
	// the client's session info
	struct session_info m_session_info;

public:
	// the client has sent a message, and it is ready to be read
	// throws if called
	void ready(SocketServer& server) override;

	// check if the client has permission to perform an op
	// this always returns true
	bool has_perm(operation_t op) const override;

	// return the client's session info
	struct session_info& session_info() override;

	// request to close the client
	// throws if called
	void close() override;

	// read from the socket
	// throws if called
	size_t read(char* buffer, size_t buffer_size) const override;

	// write a response to the client
	// this does exactly nothing
	void write(SocketServer& server, Response&& response) override;

};

} // namespace vanity::wal

#endif //VANITY_WAL_RECOVERY_CLIENT_H

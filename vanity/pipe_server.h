//
// Created by kingsli on 11/15/23.
//

#ifndef VANITY_PIPE_SERVER_H
#define VANITY_PIPE_SERVER_H

#include "auth/serial_auth_server.h"
#include "socket/socket_server.h"

namespace vanity {

/*
 * A PipeServer is a server that handles PIPE requests
 */
class PipeServer : public virtual SerialAuthServer, public virtual SocketServer
{
public:
	// a pipe request was received from a client
	void request_pipe(Client& client, const std::string& msg, size_t& pos) override;

	// an add_user request was received from a client
	void request_add_user(Client& client, const std::string& username, const std::string& password) override;

	// an edit_user request was received from a client
	void request_edit_user(Client& client, const std::string& username, client_auth auth_level) override;

	// a del_user request was received from a client
	void request_del_user(Client& client, const std::string& username) override;

	// an auth request was received from a client
	void request_auth(Client& client, const std::string& username, const std::string& password) override;

	// a change_password request was received from a client
	void request_change_password(Client& client, const std::string& new_password) override;
};

} // namespace vanity

#endif //VANITY_PIPE_SERVER_H

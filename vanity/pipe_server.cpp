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

void PipeServer::request_add_user(Client &client, const std::string &username, const std::string &password) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("ADD_USER command not allowed in PIPE request"));
	SerialAuthServer::request_add_user(client, username, password);
}

void PipeServer::request_edit_user(Client &client, const std::string &username, client_auth auth_level) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("EDIT_USER command not allowed in PIPE request"));
	SerialAuthServer::request_edit_user(client, username, auth_level);
}

void PipeServer::request_del_user(Client &client, const std::string &username) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("DEL_USER command not allowed in PIPE request"));
	SerialAuthServer::request_del_user(client, username);
}

void PipeServer::request_auth(Client &client, const std::string &username, const std::string &password) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("AUTH command not allowed in PIPE request"));
	SerialAuthServer::request_auth(client, username, password);
}

void PipeServer::request_change_password(Client &client, const std::string &new_password) {
	if (dynamic_cast<PipedClient*>(&client))
		return send(client, error("CHANGE_PASSWORD command not allowed in PIPE request"));
	SerialAuthServer::request_change_password(client, new_password);
}

} // namespace vanity
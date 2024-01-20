//
// Created by kingsli on 1/20/24.
//

#include "locked_auth_server.h"

namespace vanity {

void LockedAuthServer::request_add_user(Client &client, const std::string &username, const std::string &password) {
	std::lock_guard<std::mutex> lock{m_mutex};
	AuthServer::request_add_user(client, username, password);
}

void LockedAuthServer::request_edit_user(Client &client, const std::string &username, client_auth auth_level) {
	std::lock_guard<std::mutex> lock{m_mutex};
	AuthServer::request_edit_user(client, username, auth_level);
}

void LockedAuthServer::request_del_user(Client &client, const std::string &username) {
	std::lock_guard<std::mutex> lock{m_mutex};
	AuthServer::request_del_user(client, username);
}

void LockedAuthServer::request_auth(Client &client, const std::string &username, const std::string &password) {
	std::lock_guard<std::mutex> lock{m_mutex};
	AuthServer::request_auth(client, username, password);
}

void LockedAuthServer::request_change_password(Client &client, const std::string &new_password) {
	std::lock_guard<std::mutex> lock{m_mutex};
	AuthServer::request_change_password(client, new_password);
}

} // namespace vanity

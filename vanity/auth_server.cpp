//
// Created by kingsli on 10/10/23.
//

#include "auth_server.h"

namespace vanity {

void AuthServer::request_add_user(const Client &client, const std::string &username, const std::string &password) {
	if (username.length() < M_MIN_USERNAME_LENGTH) {
		send(client, server_constants::error + std::string{": username too short"});
		return;
	}

	if (password.length() < M_MIN_PASSWORD_LENGTH) {
		send(client, server_constants::error + std::string{": password too short"});
		return;
	}

	if (m_logins.contains(username)) {
		send(client, server_constants::error + std::string{": user already exists"});
		return;
	}

	m_logins[username] = {m_hash_function(password), client_auth::USER};
	logger().info("added user: " + username);
	send(client, server_constants::ok);
}

void AuthServer::request_edit_user(const Client &client, const std::string &username, client_auth auth_level) {
	if (not m_logins.contains(username)) {
		send(client, server_constants::error + std::string{": user does not exist"});
		return;
	}

	switch (auth_level) {
		case client_auth::USER:
		case client_auth::ADMIN:
			break;
		default:
			send(client, server_constants::error + std::string{": invalid auth level"});
			return;
	}

	m_logins[username].auth = auth_level;
	for (auto& c : m_logins[username].active) {
		c->set_auth(auth_level);
	}

	if (auth_level == client_auth::USER)
		logger().info("User " + client.username() + " made " + username + " a USER");
	else
		logger().info("User " + client.username() + " made " + username + " an ADMIN");

	send(client, server_constants::ok);
}

void AuthServer::request_del_user(const Client &client, const std::string &username) {
	if (not m_logins.contains(username)) {
		send(client, server_constants::error + std::string{": user does not exist"});
		return;
	}

	if (username == client.username()) {
		send(client, server_constants::error + std::string{": cannot delete self"});
		return;
	}

	m_logins.erase(username); // TODO: disconnect all connected clients
	logger().info("deleted user: " + username);
	send(client, server_constants::ok);
}

void AuthServer::request_auth(const Client &client, const std::string &username, const std::string &password) {
	if (password.length() < M_MIN_PASSWORD_LENGTH){
		send(client, server_constants::error + std::string{": password too short"});
		return;
	}

	if (not m_logins.contains(username)) {
		send(client, server_constants::error + std::string{": user does not exist"});
		return;
	}

	if (m_logins[username].hash != m_hash_function(password)) {
		send(client, server_constants::error + std::string{": incorrect password"});
		return;
	}

	client.username(username);
	client.set_auth(m_logins[username].auth);
	m_logins[username].active.push_back(&client);
	logger().info("authenticated user: " + username);
	send(client, server_constants::ok);
}

void AuthServer::request_change_password(const Client &client, const std::string &new_password) {
	if (new_password.length() < M_MIN_PASSWORD_LENGTH) {
		send(client, server_constants::error + std::string{": password too short"});
		return;
	}

	if (not m_logins.contains(client.username())) {
		send(client, server_constants::error + std::string{": user does not exist"});
		return;
	}

	m_logins[client.username()].hash = m_hash_function(new_password);
	logger().info("changed password for user: " + client.username());
	send(client, server_constants::ok);
}

} // namespace vanity
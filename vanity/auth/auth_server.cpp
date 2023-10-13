//
// Created by kingsli on 10/10/23.
//

#include "auth_server.h"

namespace vanity {

void AuthServer::request_add_user(const Client &client, const std::string &username, const std::string &password) {
	if (username.length() < M_MIN_USERNAME_LENGTH)
		return send_error(client, "username too short");

	if (password.length() < M_MIN_PASSWORD_LENGTH)
		return send_error(client, "password too short");

	if (m_logins.contains(username))
		return send_error(client, "user already exists");

	m_logins[username] = {m_hash_function(password), client_auth::USER};
	logger().info("added user: " + username);
	send_ok(client);
}

void AuthServer::request_edit_user(const Client &client, const std::string &username, client_auth auth_level) {
	if (not m_logins.contains(username))
		return send_error(client, "user does not exist");

	switch (auth_level) {
		case client_auth::USER:
		case client_auth::ADMIN:
			break;
		default:
			return send_error(client, "invalid auth level");
	}

	m_logins[username].auth = auth_level;
	for (auto& c : m_logins[username].active)
		c->set_auth(auth_level);

	if (auth_level == client_auth::USER)
		logger().info("User " + client.username() + " made " + username + " a USER");
	else
		logger().info("User " + client.username() + " made " + username + " an ADMIN");

	send_ok(client);
}

void AuthServer::request_del_user(const Client &client, const std::string &username) {
	if (not m_logins.contains(username))
		return send_error(client, "user does not exist");

	if (username == client.username())
		return send_error(client, "cannot delete self");

	m_logins.erase(username);
	for (auto& c : m_logins[username].active)
		c->close();

	logger().info("deleted user: " + username);
	send_ok(client);
}

void AuthServer::request_auth(const Client &client, const std::string &username, const std::string &password) {
	if (password.length() < M_MIN_PASSWORD_LENGTH)
		return send_error(client, "password too short");

	if (not m_logins.contains(username))
		return send_error(client, "user does not exist");

	if (m_logins[username].hash != m_hash_function(password))
		return send_error(client, "incorrect password");

	client.username(username);
	client.set_auth(m_logins[username].auth);
	m_logins[username].active.push_back(&client);

	logger().info("authenticated user: " + username);
	send_ok(client);
}

void AuthServer::request_change_password(const Client &client, const std::string &new_password) {
	if (new_password.length() < M_MIN_PASSWORD_LENGTH)
		return send_error(client, "password too short");

	if (not m_logins.contains(client.username()))
		return send_error(client, "user does not exist");

	m_logins[client.username()].hash = m_hash_function(new_password);
	logger().info("changed password for user: " + client.username());
	send_ok(client);
}

} // namespace vanity
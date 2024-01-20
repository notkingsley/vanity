//
// Created by kingsli on 10/10/23.
//

#include "auth_server.h"
#include "utils/serializer.h"

namespace vanity {

// write an auth_info to the output stream
template<>
void serializer::write(std::ofstream& out, const auth_info& value)
{
	write(out, value.hash);
	write(out, value.auth);
}

// read an auth_info from the input stream
template<>
auth_info serializer::read(std::ifstream& in)
{
	auth_info value;
	value.hash = read<std::string>(in);
	value.auth = read<client_auth>(in);
	return value;
}

// write a std::unordered_map<std::string, auth_info> to the output stream
template<>
void serializer::write(std::ofstream& out, const std::unordered_map<std::string, auth_info>& value)
{
	write(out, value.size());
	for (const auto& [key, val] : value) {
		write(out, key);
		write(out, val);
	}
}

// read a std::unordered_map<std::string, auth_info> from the input stream
template<>
std::unordered_map<std::string, auth_info> serializer::read(std::ifstream& in)
{
	std::unordered_map<std::string, auth_info> value;
	auto size = read<std::streamsize>(in);
	for (size_t i = 0; i < size; ++i) {
		auto key = read<std::string>(in);
		auto val = read<auth_info>(in);
		value.emplace(std::move(key), std::move(val));
	}
	return value;
}

AuthServer::AuthServer(std::optional<std::filesystem::path> users_db) noexcept
		: m_users_db{std::move(users_db)}
{
	if (m_users_db and std::filesystem::exists(m_users_db.value())) {
		std::ifstream in{m_users_db.value(), std::ios::binary};
		m_logins = serializer::read<std::unordered_map<std::string, auth_info>>(in);
		in.close();
		logger().info("Loaded logins from " + m_users_db.value().string());
	}
}

void AuthServer::persist_logins() const {
	if (not m_users_db)
		return;

	auto tmp {m_users_db.value()};
	tmp.replace_filename("tmp." + tmp.filename().string());
	std::ofstream out{tmp, std::ios::binary};
	serializer::write(out, m_logins);
	out.close();
	std::filesystem::rename(tmp, m_users_db.value());

	logger().info("Persisted logins to " + m_users_db.value().string());
}

void AuthServer::request_add_user(Client &client, const std::string &username, const std::string &password) {
	if (username.length() < M_MIN_USERNAME_LENGTH)
		return send(client, error("username too short"));

	if (password.length() < M_MIN_PASSWORD_LENGTH)
		return send(client, error("password too short"));

	if (m_logins.contains(username))
		return send(client, error("user already exists"));


	m_logins[username] = {make_hash(password), client_auth::USER};
	persist_logins();

	logger().info("added user: " + username);
	return send(client, ok());
}

void AuthServer::request_edit_user(Client &client, const std::string &username, client_auth auth_level) {
	if (not m_logins.contains(username))
		return send(client, error("user does not exist"));

	switch (auth_level) {
		case client_auth::USER:
		case client_auth::ADMIN:
			break;
		default:
			return send(client, error("invalid auth level"));
	}

	m_logins[username].auth = auth_level;
	persist_logins();

	auto log_message = "User " + session_username(client) + " made " + username;
	if (auth_level == client_auth::USER)
		log_message += " a USER";
	else
		log_message += " an ADMIN";
	logger().info(log_message);

	send(client, ok());
}

void AuthServer::request_del_user(Client &client, const std::string &username) {
	if (not m_logins.contains(username))
		return send(client, error("user does not exist"));

	if (username == session_username(client))
		return send(client, error("cannot delete self"));

	m_logins.erase(username);
	persist_logins();

	logger().info("deleted user: " + username);
	send(client, ok());
}

void AuthServer::request_auth(Client &client, const std::string &username, const std::string &password) {
	if (password.length() < M_MIN_PASSWORD_LENGTH)
		return send(client, error("password too short"));

	if (not m_logins.contains(username))
		return send(client, error("user does not exist"));

	if (not check_hash(password, m_logins[username].hash))
		return send(client, error("incorrect password"));

	session_username(client) = username;
	session_auth(client) = m_logins[username].auth;

	logger().info("authenticated user: " + username);
	send(client, ok());
}

void AuthServer::request_change_password(Client &client, const std::string &new_password) {
	if (new_password.length() < M_MIN_PASSWORD_LENGTH)
		return send(client, error("password too short"));

	auto& username = session_username(client);
	if (not m_logins.contains(username))
		return send(client, error("user does not exist"));

	m_logins[username].hash = make_hash(new_password);
	persist_logins();

	logger().info("changed password for user: " + username);
	send(client, ok());
}

auth_info AuthServer::default_auth_info() noexcept {
	const char* password = std::getenv(M_DEFAULT_PASSWORD_ENV);
	if (not password)
		password = M_DEFAULT_PASSWORD;

	return {make_hash(password), client_auth::ADMIN};
}

} // namespace vanity
//
// Created by kingsli on 10/10/23.
//

#include "auth_server.h"

namespace vanity {

void AuthServer::request_add_user(Client &client, const std::string &username, const std::string &password) {
	if (username.length() < M_MIN_USERNAME_LENGTH)
		return send_error(client, "username too short");

	if (password.length() < M_MIN_PASSWORD_LENGTH)
		return send_error(client, "password too short");

	if (m_logins.contains(username))
		return send_error(client, "user already exists");

	m_logins[username] = {make_hash(password), client_auth::USER};
	persist_logins();

	logger().info("added user: " + username);
	send_ok(client);
}

void AuthServer::request_edit_user(Client &client, const std::string &username, client_auth auth_level) {
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
	persist_logins();

	if (auth_level == client_auth::USER)
		logger().info("User " + client.username() + " made " + username + " a USER");
	else
		logger().info("User " + client.username() + " made " + username + " an ADMIN");
	send_ok(client);
}

void AuthServer::request_del_user(Client &client, const std::string &username) {
	if (not m_logins.contains(username))
		return send_error(client, "user does not exist");

	if (username == client.username())
		return send_error(client, "cannot delete self");

	for (auto& c : m_logins[username].active)
		c->close();
	m_logins.erase(username);
	persist_logins();

	logger().info("deleted user: " + username);
	send_ok(client);
}

void AuthServer::request_auth(Client &client, const std::string &username, const std::string &password) {
	if (password.length() < M_MIN_PASSWORD_LENGTH)
		return send_error(client, "password too short");

	if (not m_logins.contains(username))
		return send_error(client, "user does not exist");

	if (not check_hash(password, m_logins[username].hash))
		return send_error(client, "incorrect password");

	client.username(username);
	client.set_auth(m_logins[username].auth);
	m_logins[username].active.push_back(&client);

	logger().info("authenticated user: " + username);
	send_ok(client);
}

void AuthServer::request_change_password(Client &client, const std::string &new_password) {
	if (new_password.length() < M_MIN_PASSWORD_LENGTH)
		return send_error(client, "password too short");

	if (not m_logins.contains(client.username()))
		return send_error(client, "user does not exist");

	m_logins[client.username()].hash = make_hash(new_password);
	persist_logins();

	logger().info("changed password for user: " + client.username());
	send_ok(client);
}

// write something to an output stream
template<typename T>
void write(std::ofstream &out, const T& value);

// read something from an input stream
template<typename T>
T read(std::ifstream &in);

// write a size_t to the output stream
template<>
void write(std::ofstream &out, const size_t& value)
{
	out.write(reinterpret_cast<const char *>(&value), sizeof(value));
}

// read a size_t from the input stream
template<>
size_t read(std::ifstream &in)
{
	size_t value;
	in.read(reinterpret_cast<char *>(&value), sizeof(value));
	return value;
}

// write a long to the output stream
template<>
void write(std::ofstream &out, const long& value)
{
	out.write(reinterpret_cast<const char *>(&value), sizeof(value));
}

// read a long from the input stream
template<>
long read(std::ifstream &in)
{
	long value;
	in.read(reinterpret_cast<char *>(&value), sizeof(value));
	return value;
}

// write a client_auth to the output stream
template<>
void write(std::ofstream& out, const client_auth& value)
{
	out.write(reinterpret_cast<const char *>(&value), sizeof(value));
}

// read a client_auth from the input stream
template<>
client_auth read(std::ifstream& in)
{
	client_auth value;
	in.read(reinterpret_cast<char *>(&value), sizeof(value));
	return value;
}

// write a string to the output stream
template<>
void write(std::ofstream &out, const std::string& value)
{
	write(out, value.size());
	out.write(value.data(), value.size());
}

// read a string from the input stream
template<>
std::string read<std::string>(std::ifstream &in)
{
	auto size = read<std::streamsize>(in);
	std::string str{};
	str.resize(size);
	in.read(str.data(), size);
	return str;
}

// write an auth_info to the output stream
template<>
void write(std::ofstream& out, const auth_info& value)
{
	write(out, value.hash);
	write(out, value.auth);
}

// read an auth_info from the input stream
template<>
auth_info read(std::ifstream& in)
{
	auth_info value;
	value.hash = read<std::string>(in);
	value.auth = read<client_auth>(in);
	return value;
}

// write a std::unordered_map<std::string, auth_info> to the output stream
template<>
void write(std::ofstream& out, const std::unordered_map<std::string, auth_info>& value)
{
	write(out, value.size());
	for (const auto& [key, val] : value) {
		write(out, key);
		write(out, val);
	}
}

// read a std::unordered_map<std::string, auth_info> from the input stream
template<>
std::unordered_map<std::string, auth_info> read(std::ifstream& in)
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
		m_logins = read<std::unordered_map<std::string, auth_info>>(in);
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
	write(out, m_logins);
	out.close();
	std::filesystem::rename(tmp, m_users_db.value());

	logger().info("Persisted logins to " + m_users_db.value().string());
}

} // namespace vanity
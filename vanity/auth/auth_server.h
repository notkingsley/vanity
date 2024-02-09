//
// Created by kingsli on 10/10/23.
//

#ifndef VANITY_AUTH_SERVER_H
#define VANITY_AUTH_SERVER_H

#include <unordered_map>

#include "hasher.h"
#include "log_server.h"
#include "request/request_server.h"

namespace vanity {

struct auth_info {
	std::string hash;
	client_auth auth;
};

/*
 * An AuthServer allows authentication of clients/peers and related operations
 */
class AuthServer : public virtual RequestServer, private Hasher
{
private:
	// the default username
	static constexpr const char* M_DEFAULT_USERNAME = "vanity";

	// the default password
	static constexpr const char* M_DEFAULT_PASSWORD = "vanity";

	// the environment variable to get the default password from
	static constexpr const char* M_DEFAULT_PASSWORD_ENV = "VANITY_DEFAULT_PASSWORD";

	// the minimum username length
	static constexpr int M_MIN_USERNAME_LENGTH = 4;

	// minimum password length
	static constexpr int M_MIN_PASSWORD_LENGTH = 4;

	// this is a map of currently recognized logins
	// it maps a username to the hash of a password
	std::unordered_map<std::string, auth_info> m_logins = {
		{M_DEFAULT_USERNAME, default_auth_info()}
	};

	// file to persist the database to, if any
	const std::optional<std::filesystem::path> m_users_db;

	// persist the login data to disk
	void persist_logins();

	// get the default login info
	static auth_info default_auth_info() noexcept;

public:
	// create an AuthServer with an optional persistence file
	explicit AuthServer(std::optional<std::filesystem::path> users_db) noexcept;

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

	// an auth_level request was received from a client
	void request_auth_level(Client& client) override;

	// a username request was received from a client
	void request_username(Client& client) override;
};

} // namespace vanity

#endif //VANITY_AUTH_SERVER_H

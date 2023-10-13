//
// Created by kingsli on 10/10/23.
//

#ifndef VANITY_AUTH_SERVER_H
#define VANITY_AUTH_SERVER_H

#include <unordered_map>

#include "logging.h"
#include "request_server.h"

namespace vanity {

/*
 * An AuthServer allows authentication of clients/peers and related operations
 */
class AuthServer : public virtual RequestServer, protected virtual Logger
{
private:
	using hash_type = unsigned long;

	using hash_function_t = std::hash<std::string>;

	struct login_info_t {
		hash_type hash;
		client_auth auth;
		std::vector<const Client*> active;
	};

	// the default username
	static constexpr const char* M_DEFAULT_USERNAME = "vanity";

	// the default password
	static constexpr const char* M_DEFAULT_PASSWORD = "vanity";

	// the minimum username length
	static constexpr int M_MIN_USERNAME_LENGTH = 4;

	// minimum password length
	static constexpr int M_MIN_PASSWORD_LENGTH = 4;

	// the hash function to use
	static constexpr hash_function_t m_hash_function{};

	// this is a map of currently recognized logins
	// it maps a username to the hash of a password
	std::unordered_map<std::string, login_info_t> m_logins = {
		{M_DEFAULT_USERNAME, {m_hash_function(M_DEFAULT_PASSWORD), client_auth::ADMIN}}
	};

public:
	// an add_user request was received from a client
	void request_add_user(const Client& client, const std::string& username, const std::string& password) override;

	// an edit_user request was received from a client
	void request_edit_user(const Client& client, const std::string& username, client_auth auth_level) override;

	// a del_user request was received from a client
	void request_del_user(const Client& client, const std::string& username) override;

	// an auth request was received from a client
	void request_auth(const Client& client, const std::string& username, const std::string& password) override;

	// a change_password request was received from a client
	void request_change_password(const Client& client, const std::string& new_password) override;

private:
	// persist the login data to disk
	void persist_logins() const;
};

} // namespace vanity

#endif //VANITY_AUTH_SERVER_H

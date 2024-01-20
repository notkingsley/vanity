//
// Created by kingsli on 1/20/24.
//

#ifndef VANITY_LOCKED_AUTH_SERVER_H
#define VANITY_LOCKED_AUTH_SERVER_H

#include "auth_server.h"

namespace vanity {

/*
 * A LockedAuthServer is an AuthServer that locks all operations
 */
class LockedAuthServer : public virtual AuthServer
{
private:
	// the mutex used to lock all operations
	std::mutex m_mutex;

public:
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

#endif //VANITY_LOCKED_AUTH_SERVER_H

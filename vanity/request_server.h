//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_REQUEST_SERVER_H
#define VANITY_REQUEST_SERVER_H


#include "abstract_server.h"
#include "logging.h"


namespace vanity {

/*
 * A RequestServer allows to process incoming requests
 * and dispatch them to the appropriate handler
 */
class RequestServer : public virtual AbstractServer, public virtual Logger
{
public:
	// a message was received from a client
	void handle(const std::string& msg, Client& client) override;

	// a get request was received from a client
	virtual void request_get(Client& client, const std::string& key) = 0;

	// a set request was received from a client
	virtual void request_set(Client& client, const std::string& key, const std::string& value) = 0;

	// a set request was received from a client
	virtual void request_set(Client& client, const std::string& key, const int64_t& value) = 0;

	// a set request was received from a client
	virtual void request_set(Client& client, const std::string& key, const double& value) = 0;

	// a del request was received from a client
	virtual void request_del(Client& client, const std::string& key) = 0;

	// a type request was received from a client
	virtual void request_type(Client& client, const std::string& key) = 0;

	// an exists request was received from a client
	virtual void request_exists(Client& client, const std::string& key) = 0;

	// an incr_int request was received from a client
	virtual void request_incr_int(Client& client, const std::string& key, int64_t value) = 0;

	// an incr_float request was received from a client
	virtual void request_incr_float(Client& client, const std::string& key, double value) = 0;

	// a len_str request was received from a client
	virtual void request_len_str(Client& client, const std::string& key) = 0;

	// a persist request was received from a client
	virtual void request_persist(Client& client) = 0;

	// a reset request was received from a client
	virtual void request_reset(Client& client) = 0;

	// a switch_db request was received from a client
	virtual void request_switch_db(Client& client, int64_t index) = 0;

	// an add_user request was received from a client
	virtual void request_add_user(Client& client, const std::string& username, const std::string& password) = 0;

	// an edit_user request was received from a client
	virtual void request_edit_user(Client& client, const std::string& username, client_auth auth_level) = 0;

	// a del_user request was received from a client
	virtual void request_del_user(Client& client, const std::string& username) = 0;

	// an auth request was received from a client
	virtual void request_auth(Client& client, const std::string& username, const std::string& password) = 0;

	// a change_password request was received from a client
	virtual void request_change_password(Client& client, const std::string& password) = 0;

	// an exit request was received from a client
	virtual void request_exit(Client& client) {
		throw DestroyClient{};
	};

	// a terminate request was received from a client
	virtual void request_terminate(Client& client) {
		terminate();
	};

	// a ping request was received from a client
	virtual void request_ping(Client& client, const std::string& msg) {
		send(client, pong(msg));
	};

private:
	// convenience function to dispatch a set request by type
	void dispatch_set(Client& client, const std::string& msg, size_t& pos);
};

} // namespace vanity

#endif //VANITY_REQUEST_SERVER_H

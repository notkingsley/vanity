//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_REQUEST_SERVER_H
#define VANITY_REQUEST_SERVER_H

#include <list>
#include <unordered_set>

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

	// a str_len request was received from a client
	virtual void request_str_len(Client& client, const std::string& key) = 0;

	// a many_get request was received from a client
	virtual void request_many_get(Client& client, const std::vector<std::string>& keys) = 0;

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

	// a list_len request was received from a client
	virtual void request_list_len(Client& client, const std::string& key) = 0;

	// a list_get request was received from a client
	virtual void request_list_get(Client& client, const std::string& key, int64_t index) = 0;

	// a list_set request was received from a client
	virtual void request_list_set(Client& client, const std::string& key, int64_t index, const std::string& value) = 0;

	// a list_push_left request was received from a client
	virtual void request_list_push_left(Client& client, const std::string& key, std::list<std::string> values) = 0;

	// a list_push_right request was received from a client
	virtual void request_list_push_right(Client& client, const std::string& key, std::list<std::string> values) = 0;

	// a list_pop_left request was received from a client
	virtual void request_list_pop_left(Client& client, const std::string& key, int64_t count) = 0;

	// a list_pop_right request was received from a client
	virtual void request_list_pop_right(Client& client, const std::string& key, int64_t count) = 0;

	// a list_range request was received from a client
	virtual void request_list_range(Client& client, const std::string& key, int64_t start, int64_t stop) = 0;

	// a list_trim request was received from a client
	virtual void request_list_trim(Client& client, const std::string& key, int64_t start, int64_t stop) = 0;

	// a list_remove request was received from a client
	virtual void request_list_remove(Client& client, const std::string& key, const std::string& value, int64_t count) = 0;

	// a set_add request was received from a client
	virtual void request_set_add(Client& client, const std::string& key, std::unordered_set<std::string> values) = 0;

	// a set_all request was received from a client
	virtual void request_set_all(Client& client, const std::string& key) = 0;

	// a set_remove request was received from a client
	virtual void request_set_remove(Client& client, const std::string& key, int64_t count) = 0;

	// a set_discard request was received from a client
	virtual void request_set_discard(Client& client, const std::string& key, std::unordered_set<std::string> values) = 0;

	// a set_len request was received from a client
	virtual void request_set_len(Client& client, const std::string& key) = 0;

	// a set_contains request was received from a client
	virtual void request_set_contains(Client& client, const std::string& key, const std::string& value) = 0;

	// a set_move request was received from a client
	virtual void request_set_move(Client& client, const std::string& source, const std::string& dest, const std::string& value) = 0;

	// a set_union request was received from a client
	virtual void request_set_union(Client& client, std::vector<std::string> keys) = 0;

	// a set_union_into request was received from a client
	virtual void request_set_union_into(Client& client, const std::string& dest, std::vector<std::string> keys) = 0;

	// a set_union_len request was received from a client
	virtual void request_set_union_len(Client& client, std::vector<std::string> keys) = 0;

	// a set_intersection request was received from a client
	virtual void request_set_intersection(Client& client, std::vector<std::string> keys) = 0;

	// a set_intersection_into request was received from a client
	virtual void request_set_intersection_into(Client& client, const std::string& dest, std::vector<std::string> keys) = 0;

	// a set_intersection_len request was received from a client
	virtual void request_set_intersection_len(Client& client, std::vector<std::string> keys) = 0;

	// a set_difference request was received from a client
	virtual void request_set_difference(Client& client, const std::string& key1, const std::string& key2) = 0;

	// a set_difference_into request was received from a client
	virtual void request_set_difference_into(Client& client, const std::string& dest, const std::string& key1, const std::string& key2) = 0;

	// a set_difference_len request was received from a client
	virtual void request_set_difference_len(Client& client, const std::string& key1, const std::string& key2) = 0;

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

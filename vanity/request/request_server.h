//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_REQUEST_SERVER_H
#define VANITY_REQUEST_SERVER_H

#include <list>
#include <unordered_set>

#include "abstract_server.h"
#include "log_server.h"
#include "request.h"
#include "response/response_server.h"
#include "wal/wal_write_server.h"


namespace vanity {

/*
 * A RequestServer allows to process incoming requests
 * and dispatch them to the appropriate handler
 */
class RequestServer :
	public virtual AbstractServer,
	public virtual ResponseServer,
	public virtual LogServer,
	public virtual WalWriteServer
{
public:
	// a pipe request was received from a client
	virtual void request_pipe(Client& client, Request& request) = 0;

	// a bind request was received from a client
	virtual void request_bind(Client& client, const std::string& host, int64_t port) = 0;

	// an unbind request was received from a client
	virtual void request_unbind(Client& client, const std::string& host, int64_t port) = 0;

	// a get request was received from a client
	virtual void request_get(Client& client, const std::string& key) = 0;

	// a str_set request was received from a client
	virtual void request_str_set(Client& client, const std::string& key, const std::string& value) = 0;

	// an int_set request was received from a client
	virtual void request_int_set(Client& client, const std::string& key, const int64_t& value) = 0;

	// a float_set request was received from a client
	virtual void request_float_set(Client& client, const std::string& key, const double& value) = 0;

	// a del request was received from a client
	virtual void request_del(Client& client, const std::string& key) = 0;

	// a type request was received from a client
	virtual void request_type(Client& client, const std::string& key) = 0;

	// an exists request was received from a client
	virtual void request_exists(Client& client, const std::string& key) = 0;

	// a keys request was received from a client
	virtual void request_keys(Client& client) = 0;

	// a copy_to request was received from a client
	virtual void request_copy_to(Client& client, const std::string& key, const std::string& dest) = 0;

	// a move_to request was received from a client
	virtual void request_move_to(Client& client, const std::string& key, const std::string& dest) = 0;

	// a copy_to_db request was received from a client
	virtual void request_copy_to_db(Client& client, const std::string& key, int64_t dest) = 0;

	// a move_to_db request was received from a client
	virtual void request_move_to_db(Client& client, const std::string& key, int64_t dest) = 0;

	// a set_expiry request was received from a client
	virtual void request_set_expiry(Client& client, const std::string& key, double seconds) = 0;

	// a get_expiry request was received from a client
	virtual void request_get_expiry(Client& client, const std::string& key) = 0;

	// a clear_expiry request was received from a client
	virtual void request_clear_expiry(Client& client, const std::string& key) = 0;

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

	// an auth_level request was received from a client
	virtual void request_auth_level(Client& client) = 0;

	// a username request was received from a client
	virtual void request_username(Client& client) = 0;

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

	// a hash_set request was received from a client
	virtual void request_hash_set(Client& client, const std::string& key, std::unordered_map<std::string, std::string> values) = 0;

	// a hash_all request was received from a client
	virtual void request_hash_all(Client& client, const std::string& key) = 0;

	// a hash_get request was received from a client
	virtual void request_hash_get(Client& client, const std::string& key, const std::string& hash_key) = 0;

	// a hash_contains request was received from a client
	virtual void request_hash_contains(Client& client, const std::string& key, const std::string& hash_key) = 0;

	// a hash_len request was received from a client
	virtual void request_hash_len(Client& client, const std::string& key) = 0;

	// a hash_key_len request was received from a client
	virtual void request_hash_key_len(Client& client, const std::string& key, const std::string& hash_key) = 0;

	// a hash_remove request was received from a client
	virtual void request_hash_remove(Client& client, const std::string& key, const std::vector<std::string>& hash_keys) = 0;

	// a hash_keys request was received from a client
	virtual void request_hash_keys(Client& client, const std::string& key) = 0;

	// a hash_values request was received from a client
	virtual void request_hash_values(Client& client, const std::string& key) = 0;

	// a hash_update request was received from a client
	virtual void request_hash_update(Client& client, const std::string& key, std::unordered_map<std::string, std::string> values) = 0;

	// a hash_many_get request was received from a client
	virtual void request_hash_many_get(Client& client, const std::string& key, const std::vector<std::string>& hash_keys) = 0;

	// a transact_begin request was received from a client
	virtual void request_transact_begin(Client& client) = 0;

	// a transact_commit request was received from a client
	virtual void request_transact_commit(Client& client) = 0;

	// a transact_discard request was received from a client
	virtual void request_transact_discard(Client& client) = 0;

	// a subscribe request was received from a client
	virtual void request_subscribe(Client& client, const std::string& channel) = 0;

	// a subscribed request was received from a client
	virtual void request_subscribed(Client& client) = 0;

	// an unsubscribe request was received from a client
	virtual void request_unsubscribe(Client& client, const std::string& channel) = 0;

	// an unsubscribe_all request was received from a client
	virtual void request_unsubscribe_all(Client& client) = 0;

	// a publish request was received from a client
	virtual void request_publish(Client& client, const std::string& channel, const std::string& message) = 0;

	// a cluster_join request was received from a client
	virtual void request_cluster_join(Client& client, const std::string& key, const std::string& host, uint16_t port) = 0;

	// a cluster_leave request was received from a client
	virtual void request_cluster_leave(Client& client) = 0;

	// a cluster_key request was received from a client
	virtual void request_cluster_key(Client& client) = 0;

	// a cluster_new request was received from a client
	virtual void request_cluster_new(Client& client, const std::string& key) = 0;

	// a peers request was received from a client
	virtual void request_peers(Client& client) = 0;

	// a peer_auth request was received from a client
	virtual void request_peer_auth(Client& client, const std::string& key, const std::string& addr) = 0;

	// an exit request was received from a client
	virtual void request_exit(Client& client) {
		throw DestroyClient{};
	};

	// a terminate request was received from a client
	virtual void request_terminate(Client& client) {
		terminate();
	};

	// a ping request was received from a client
	virtual void request_ping(Client& client) {
		send(client, pong());
	};

protected:
	// redo a previously WALed request
	void wal_redo_request(const std::string& request, Client &client);

	// a message was received from a user client
	void handle_user(const std::string& msg, Client& client);

	// dispatch multiple requests using do_handle, expecting the message to end after the last one
	// this calls do_handle_single on each request in the message,
	// so a response will be sent for each one no matter what
	// behavior is undefined if one of the requests couldn't be dispatched
	void do_handle(Client& client, Request& request, size_t len);

	// extract the data from a request and dispatch it to the appropriate handler
	// client: the client that sent the message
	// request: the request to extract data from
	// end: whether the message should end after this operation. an error is sent if it doesn't
	// return true if the request was extracted and dispatched successfully, false otherwise
	bool do_handle_single(Client& client, Request& request, bool end);

	// dispatch a request based on the client's behavior
	// this selects the correct handler based on the client's current state
	// effectively same as do_handle, but doesn't catch errors or guarantee a response will be sent
	bool dispatch_by_behavior(Client& client, Request& request, bool end);

	// dispatch a request in the default behavior
	// this is the default behavior for a client, and is used when the client is not in any special state
	// or when the request is not affected by the client's state/behavior
	// returns true if the request was dispatched successfully, false otherwise
	bool dispatch_default_behavior(Client& client, Request& request, bool end);

	// dispatch a request not permitted in the current state
	// this is called when a request is not permitted in the current state
	// and is distinct from the request not being permitted due to the client's auth level
	// returns true if the request was dispatched successfully, false otherwise
	bool dispatch_not_permitted_behavior(Client& client, Request& request, bool end);

	// dispatch a request in a contextual behavior
	// this is called when a request is permitted in the current state, but is affected by the client's state/behavior
	// returns true if the request was dispatched successfully, false otherwise
	bool dispatch_contextual_behavior(Client& client, Request& request, bool end);

	// dispatch a request in a normal context
	bool dispatch_normal_context(Client& client, Request& request, bool end);

	// dispatch a request in a transaction context
	virtual bool dispatch_transaction_context(Client& client, Request& request, bool end) = 0;

	// send a response to the client, calling dry_dispatch if necessary
	// always returns false
	bool refuse_with_response(Client& client, Request& request, Response&& response, bool end);

	// convenience function that contains a giant switch statement to dispatch an operation_t
	void dispatch(Client& client, Request& request, bool end);

	// similar to dispatch_op, but merely advances m_pos by extracting the data without actually calling the request_ method
	static void dry_dispatch(Request& request, bool end);
};

} // namespace vanity

#endif //VANITY_REQUEST_SERVER_H

//
// Created by kingsli on 8/26/24.
//

#ifndef VANITY_REQUEST_HANDLER_H
#define VANITY_REQUEST_HANDLER_H

#include "client/client.h"
#include "request.h"


namespace vanity {

/*
 * A RequestHandler defines an abstract interface for handling requests
 */
class RequestHandler
{
public:
	// an exit request was received from a client
	virtual void request_exit(Client& client) = 0;

	// a terminate request was received from a client
	virtual void request_terminate(Client& client) = 0;

	// a ping request was received from a client
	virtual void request_ping(Client& client) = 0;

	// a pipe request was received from a client
	virtual void request_pipe(Client& client, Request& request) = 0;


	// a bind request was received from a client
	virtual void request_bind(Client& client, const std::string& host, int64_t port) = 0;

	// an unbind request was received from a client
	virtual void request_unbind(Client& client, const std::string& host, int64_t port) = 0;


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


	// a switch_db request was received from a client
	virtual void request_switch_db(Client& client, int64_t index) = 0;

	// a persist request was received from a client
	virtual void request_persist(Client& client) = 0;


	// a transact_begin request was received from a client
	virtual void request_transact_begin(Client& client) = 0;

	// a transact_commit request was received from a client
	virtual void request_transact_commit(Client& client) = 0;

	// a transact_discard request was received from a client
	virtual void request_transact_discard(Client& client) = 0;


	// a publish request was received from a client
	virtual void request_publish(Client& client, const std::string& channel, const std::string& message) = 0;

	// a subscribe request was received from a client
	virtual void request_subscribe(Client& client, const std::string& channel) = 0;

	// a subscribed request was received from a client
	virtual void request_subscribed(Client& client) = 0;

	// an unsubscribe request was received from a client
	virtual void request_unsubscribe(Client& client, const std::string& channel) = 0;

	// an unsubscribe_all request was received from a client
	virtual void request_unsubscribe_all(Client& client) = 0;


	// a del request was received from a client
	virtual void request_del(Client& client, const std::string& key) = 0;

	// a type request was received from a client
	virtual void request_type(Client& client, const std::string& key) = 0;

	// a reset request was received from a client
	virtual void request_reset(Client& client) = 0;

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


	// a get request was received from a client
	virtual void request_get(Client& client, const std::string& key) = 0;

	// a str_set request was received from a client
	virtual void request_str_set(Client& client, const std::string& key, std::string value) = 0;

	// an int_set request was received from a client
	virtual void request_int_set(Client& client, const std::string& key, int64_t value) = 0;

	// a float_set request was received from a client
	virtual void request_float_set(Client& client, const std::string& key, double value) = 0;

	// an incr_int request was received from a client
	virtual void request_incr_int(Client& client, const std::string& key, int64_t value) = 0;

	// an incr_float request was received from a client
	virtual void request_incr_float(Client& client, const std::string& key, double value) = 0;

	// a str_len request was received from a client
	virtual void request_str_len(Client& client, const std::string& key) = 0;

	// a many_get request was received from a client
	virtual void request_many_get(Client& client, const std::vector<std::string>& keys) = 0;


	// a list_len request was received from a client
	virtual void request_list_len(Client& client, const std::string& key) = 0;

	// a list_get request was received from a client
	virtual void request_list_get(Client& client, const std::string& key, int64_t index) = 0;

	// a list_set request was received from a client
	virtual void request_list_set(Client& client, const std::string& key, int64_t index, std::string value) = 0;

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


	// a cluster_join request was received from a client
	virtual void request_cluster_join(Client& client, const std::string& key, const std::string& id, const std::string& host, uint16_t port) = 0;

	// a cluster_key request was received from a client
	virtual void request_cluster_key(Client& client) = 0;

	// a cluster_id request was received from a client
	virtual void request_cluster_id(Client& client) = 0;

	// a cluster_leave request was received from a client
	virtual void request_cluster_leave(Client& client) = 0;

	// a cluster_new request was received from a client
	virtual void request_cluster_new(Client& client, const std::string& key, const std::string& id) = 0;


	// a peers request was received from a client
	virtual void request_peers(Client& client) = 0;

	// a peer_ids request was received from a client
	virtual void request_peer_ids(Client& client) = 0;

	// a peer_auth request was received from a client
	virtual void request_peer_auth(Client& client, int64_t id, const std::string& key, const std::string& addr, const std::string& peer_id) = 0;

protected:
	// convenience function that contains a giant switch statement to dispatch an operation_t
	// this extracts the command and arguments from
	// the request string and calls the appropriate request_ method
	// this is used by RequestServer to dispatch requests and follow the same pattern
	// do not call
	void dispatch(Client& client, Request& request, bool end);

	// similar to dispatch_op, but merely advances m_pos by
	// extracting the data without actually calling the request_ method
	static void dry_dispatch(Request& request, bool end);
};

} // namespace vanity

#endif //VANITY_REQUEST_HANDLER_H

//
// Created by kingsli on 9/14/23.
//

#include "request_server.h"
#include "request_tracker.h"

namespace vanity {

void RequestServer::wal_redo_request(const std::string &request, Client &client) {
	handle_user(request, client);
}

void RequestServer::handle_user(const std::string& msg, Client& client) {
	Request request{msg};
	do_handle(client, request, 1);
}

void RequestServer::do_handle(Client &client, Request &request, size_t len) {
	if (len == 0)
		return;

	for (size_t i = 0; i < len - 1; ++i)
		do_handle_single(client, request, false);
	do_handle_single(client, request, true);
}

bool RequestServer::do_handle_single(Client &client, Request& request, bool end) {
	try
	{
		return dispatch_by_behavior(client, request, end);
	}
	catch (const InvalidRequest& e)
	{
		send(client, bad_request(e.what()));
		return false;
	}
	catch (const DestroyClient& e)
	{
		logger().debug("Client disconnecting");
		throw;
	}
	catch (const Exception& e)
	{
		logger().error(e.what());
		send(client, internal_error(e.what()));
		return true;
	}
	catch (const std::exception& e)
	{
		logger().error(e.what());
		send(client, internal_error(e.what()));
		return true;
	}
	catch (...)
	{
		logger().error("unknown error in request: " + request.format());
		send(client, internal_error("unknown error"));
		return true;
	}
}

bool RequestServer::dispatch_by_behavior(Client &client, Request& request, bool end) {
	switch (behaviour(request.peek_operation(), session_state(client))) {
		case behaviour_t::DEFAULT:
		{
			return dispatch_default_behavior(client, request, end);
		}
		case behaviour_t::NOT_PERMITTED:
		{
			return dispatch_not_permitted_behavior(client, request, end);
		}
		case behaviour_t::CONTEXTUAL:
		{
			return dispatch_contextual_behavior(client, request, end);
		}
		default:
		{
			throw std::runtime_error("invalid behaviour");
		}
	}
}

bool RequestServer::dispatch_default_behavior(Client& client, Request& request, bool end) {
	return dispatch_normal_context(client, request, end);
}

bool RequestServer::dispatch_not_permitted_behavior(Client& client, Request& request, bool end) {
	return refuse_with_response(client, request, bad_state(), end);
}

bool RequestServer::dispatch_contextual_behavior(Client& client, Request& request, bool end) {
	switch (session_state(client)) {
		case conn_state::NORMAL:
		{
			return dispatch_normal_context(client, request, end);
		}
		case conn_state::TRANSACTION:
		{
			return dispatch_transaction_context(client, request, end);
		}
		default:
		{
			throw std::runtime_error("invalid state");
		}
	}
}

bool RequestServer::dispatch_normal_context(Client &client, Request& request, bool end) {
	if (not client.has_perm(request.peek_operation()))
		return refuse_with_response(client, request, denied(), end);

	dispatch(client, request, end);
	return true;
}

bool RequestServer::refuse_with_response(Client& client, Request& request, Response&& response, bool end) {
	if (not end)
		dry_dispatch(request, end);

	send(client, response.move());
	return false;
}

void RequestServer::dispatch(Client &client, Request& request, bool end) {
	using enum object_t;

	RequestTracker tracker {request};
	auto op {request.get_operation()};

	switch (op) {
		case operation_t::TERMINATE:
		{
			request.get_exact<>(true);
			wal_request(client, op, tracker.view());
			request_terminate(client);
			break;
		}
		case operation_t::EXIT:
		{
			request.get_exact<>(true);
			wal_request(client, op, tracker.view());
			request_exit(client);
			break;
		}
		case operation_t::PING:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_ping(client);
			break;
		}
		case operation_t::PIPE:
		{
			wal_request(client, op, tracker.view());
			request_pipe(client, request);
			request.get_exact<>(end);
			break;
		}

		case operation_t::AUTH:
		{
			auto [username, password] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_auth(client, username, password);
			break;
		}
		case operation_t::ADD_USER:
		{
			auto [username, password] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_add_user(client, username, password);
			break;
		}
		case operation_t::EDIT_USER:
		{
			auto [username, auth] = request.get_exact<STR, CLIENT_AUTH>(end);
			wal_request(client, op, tracker.view());
			request_edit_user(client, username, auth);
			break;
		}
		case operation_t::DEL_USER:
		{
			auto username = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_del_user(client, username);
			break;
		}
		case operation_t::CHANGE_PASSWORD:
		{
			auto password = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_change_password(client, password);
			break;
		}
		case operation_t::AUTH_LEVEL:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_auth_level(client);
			break;
		}
		case operation_t::USERNAME:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_username(client);
			break;
		}

		case operation_t::SWITCH_DB:
		{
			auto index = request.get_exact<INT>(end);
			wal_request(client, op, tracker.view());
			request_switch_db(client, index);
			break;
		}
		case operation_t::PERSIST:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_persist(client);
			break;
		}

		case operation_t::TRANSACT_BEGIN:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_transact_begin(client);
			break;
		}
		case operation_t::TRANSACT_COMMIT:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_transact_commit(client);
			break;
		}
		case operation_t::TRANSACT_DISCARD:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_transact_discard(client);
			break;
		}

		case operation_t::PUBLISH:
		{
			auto [channel, message] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_publish(client, channel, message);
			break;
		}
		case operation_t::SUBSCRIBE:
		{
			auto channel = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_subscribe(client, channel);
			break;
		}
		case operation_t::SUBSCRIBED:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_subscribed(client);
			break;
		}
		case operation_t::UNSUBSCRIBE:
		{
			auto channel = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_unsubscribe(client, channel);
			break;
		}
		case operation_t::UNSUBSCRIBE_ALL:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_unsubscribe_all(client);
			break;
		}

		case operation_t::DEL:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_del(client, key);
			break;
		}
		case operation_t::TYPE:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_type(client, key);
			break;
		}
		case operation_t::EXISTS:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_exists(client, key);
			break;
		}
		case operation_t::RESET:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_reset(client);
			break;
		}
		case operation_t::KEYS:
		{
			request.get_exact<>(end);
			wal_request(client, op, tracker.view());
			request_keys(client);
			break;
		}
		case operation_t::COPY_TO:
		{
			auto [from, to] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_copy_to(client, from, to);
			break;
		}
		case operation_t::MOVE_TO:
		{
			auto [from, to] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_move_to(client, from, to);
			break;
		}
		case operation_t::COPY_TO_DB:
		{
			auto [from, to] = request.get_exact<STR, INT>(end);
			wal_request(client, op, tracker.view());
			request_copy_to_db(client, from, to);
			break;
		}
		case operation_t::MOVE_TO_DB:
		{
			auto [from, to] = request.get_exact<STR, INT>(end);
			wal_request(client, op, tracker.view());
			request_move_to_db(client, from, to);
			break;
		}

		case operation_t::SET_EXPIRY:
		{
			auto [key, expiry] = request.get_exact<STR, FLOAT>(end);
			wal_request(client, op, tracker.view());
			request_set_expiry(client, key, expiry);
			break;
		}
		case operation_t::GET_EXPIRY:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_get_expiry(client, key);
			break;
		}
		case operation_t::CLEAR_EXPIRY:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_clear_expiry(client, key);
			break;
		}

		case operation_t::GET:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_get(client, key);
			break;
		}
		case operation_t::STR_SET:
		{
			auto [key, value] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_str_set(client, key, value);
			break;
		}
		case operation_t::INT_SET:
		{
			auto [key, value] = request.get_exact<STR, INT>(end);
			wal_request(client, op, tracker.view());
			request_int_set(client, key, value);
			break;
		}
		case operation_t::FLOAT_SET:
		{
			auto [key, value] = request.get_exact<STR, FLOAT>(end);
			wal_request(client, op, tracker.view());
			request_float_set(client, key, value);
			break;
		}
		case operation_t::INCR_INT:
		{
			auto [key, value] = request.get_exact<STR, INT>(end);
			wal_request(client, op, tracker.view());
			request_incr_int(client, key, value);
			break;
		}
		case operation_t::INCR_FLOAT:
		{
			auto [key, value] = request.get_exact<STR, FLOAT>(end);
			wal_request(client, op, tracker.view());
			request_incr_float(client, key, value);
			break;
		}
		case operation_t::STR_LEN:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_str_len(client, key);
			break;
		}
		case operation_t::MANY_GET:
		{
			auto keys = request.get_exact<ARR>(end);
			wal_request(client, op, tracker.view());
			request_many_get(client, keys);
			break;
		}

		case operation_t::LIST_LEN:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_list_len(client, key);
			break;
		}
		case operation_t::LIST_GET:
		{
			auto [key, index] = request.get_exact<STR, INT>(end);
			wal_request(client, op, tracker.view());
			request_list_get(client, key, index);
			break;
		}
		case operation_t::LIST_SET:
		{
			auto [key, index, value] = request.get_exact<STR, INT, STR>(end);
			wal_request(client, op, tracker.view());
			request_list_set(client, key, index, value);
			break;
		}
		case operation_t::LIST_PUSH_LEFT:
		{
			auto [key, value] = request.get_exact<STR, LIST>(end);
			wal_request(client, op, tracker.view());
			request_list_push_left(client, key, std::move(value));
			break;
		}
		case operation_t::LIST_PUSH_RIGHT:
		{
			auto [key, value] = request.get_exact<STR, LIST>(end);
			wal_request(client, op, tracker.view());
			request_list_push_right(client, key, std::move(value));
			break;
		}
		case operation_t::LIST_POP_LEFT:
		{
			auto [key, count] = request.get_exact<STR, INT>(end);
			wal_request(client, op, tracker.view());
			request_list_pop_left(client, key, count);
			break;
		}
		case operation_t::LIST_POP_RIGHT:
		{
			auto [key, count] = request.get_exact<STR, INT>(end);
			wal_request(client, op, tracker.view());
			request_list_pop_right(client, key, count);
			break;
		}
		case operation_t::LIST_RANGE:
		{
			auto [key, start, stop] = request.get_exact<STR, INT, INT>(end);
			wal_request(client, op, tracker.view());
			request_list_range(client, key, start, stop);
			break;
		}
		case operation_t::LIST_TRIM:
		{
			auto [key, start, stop] = request.get_exact<STR, INT, INT>(end);
			wal_request(client, op, tracker.view());
			request_list_trim(client, key, start, stop);
			break;
		}
		case operation_t::LIST_REMOVE:
		{
			auto [key, value, count] = request.get_exact<STR, STR, INT>(end);
			wal_request(client, op, tracker.view());
			request_list_remove(client, key, value, count);
			break;
		}

		case operation_t::SET_ADD:
		{
			auto [key, values] = request.get_exact<STR, SET>(end);
			wal_request(client, op, tracker.view());
			request_set_add(client, key, std::move(values));
			break;
		}
		case operation_t::SET_ALL:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_set_all(client, key);
			break;
		}
		case operation_t::SET_REMOVE:
		{
			auto [key, count] = request.get_exact<STR, INT>(end);
			wal_request(client, op, tracker.view());
			request_set_remove(client, key, count);
			break;
		}
		case operation_t::SET_DISCARD:
		{
			auto [key, values] = request.get_exact<STR, SET>(end);
			wal_request(client, op, tracker.view());
			request_set_discard(client, key, std::move(values));
			break;
		}
		case operation_t::SET_LEN:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_set_len(client, key);
			break;
		}
		case operation_t::SET_CONTAINS:
		{
			auto [key, value] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_set_contains(client, key, value);
			break;
		}
		case operation_t::SET_MOVE:
		{
			auto [source, dest, value] = request.get_exact<STR, STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_set_move(client, source, dest, value);
			break;
		}
		case operation_t::SET_UNION:
		{
			auto keys = request.get_exact<ARR>(end);
			wal_request(client, op, tracker.view());
			request_set_union(client, keys);
			break;
		}
		case operation_t::SET_UNION_INTO:
		{
			auto [dest, keys] = request.get_exact<STR, ARR>(end);
			wal_request(client, op, tracker.view());
			request_set_union_into(client, dest, keys);
			break;
		}
		case operation_t::SET_UNION_LEN:
		{
			auto keys = request.get_exact<ARR>(end);
			wal_request(client, op, tracker.view());
			request_set_union_len(client, keys);
			break;
		}
		case operation_t::SET_INTERSECT:
		{
			auto keys = request.get_exact<ARR>(end);
			wal_request(client, op, tracker.view());
			request_set_intersection(client, keys);
			break;
		}
		case operation_t::SET_INTERSECT_INTO:
		{
			auto [dest, keys] = request.get_exact<STR, ARR>(end);
			wal_request(client, op, tracker.view());
			request_set_intersection_into(client, dest, keys);
			break;
		}
		case operation_t::SET_INTERSECT_LEN:
		{
			auto keys = request.get_exact<ARR>(end);
			wal_request(client, op, tracker.view());
			request_set_intersection_len(client, keys);
			break;
		}
		case operation_t::SET_DIFF:
		{
			auto [key1, key2] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_set_difference(client, key1, key2);
			break;
		}
		case operation_t::SET_DIFF_INTO:
		{
			auto [dest, key1, key2] = request.get_exact<STR, STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_set_difference_into(client, dest, key1, key2);
			break;
		}
		case operation_t::SET_DIFF_LEN:
		{
			auto [key1, key2] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_set_difference_len(client, key1, key2);
			break;
		}

		case operation_t::HASH_SET:
		{
			auto [key, hash] = request.get_exact<STR, HASH>(end);
			wal_request(client, op, tracker.view());
			request_hash_set(client, key, std::move(hash));
			break;
		}
		case operation_t::HASH_ALL:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_hash_all(client, key);
			break;
		}
		case operation_t::HASH_GET:
		{
			auto [key, hash_key] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_hash_get(client, key, hash_key);
			break;
		}
		case operation_t::HASH_CONTAINS:
		{
			auto [key, hash_key] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_hash_contains(client, key, hash_key);
			break;
		}
		case operation_t::HASH_LEN:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_hash_len(client, key);
			break;
		}
		case operation_t::HASH_KEY_LEN:
		{
			auto [key, hash_key] = request.get_exact<STR, STR>(end);
			wal_request(client, op, tracker.view());
			request_hash_key_len(client, key, hash_key);
			break;
		}
		case operation_t::HASH_REMOVE:
		{
			auto [key, hash_key] = request.get_exact<STR, ARR>(end);
			wal_request(client, op, tracker.view());
			request_hash_remove(client, key, hash_key);
			break;
		}
		case operation_t::HASH_KEYS:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_hash_keys(client, key);
			break;
		}
		case operation_t::HASH_VALUES:
		{
			auto key = request.get_exact<STR>(end);
			wal_request(client, op, tracker.view());
			request_hash_values(client, key);
			break;
		}
		case operation_t::HASH_UPDATE:
		{
			auto [key, hash] = request.get_exact<STR, HASH>(end);
			wal_request(client, op, tracker.view());
			request_hash_update(client, key, std::move(hash));
			break;
		}
		case operation_t::HASH_MANY_GET:
		{
			auto [key, hash_keys] = request.get_exact<STR, ARR>(end);
			wal_request(client, op, tracker.view());
			request_hash_many_get(client, key, hash_keys);
			break;
		}

		case operation_t::CLUSTER_JOIN:
		{
			auto [host, port] = request.get_exact<STR, INT>(end);
			wal_request(client, op, tracker.view());
			request_cluster_join(client, host, port);
			break;
		}
	}
}

void RequestServer::dry_dispatch(Request& request, bool end) {
	using enum object_t;

	switch (request.get_operation()) {
		case operation_t::TERMINATE:
		{
			request.get_exact<>(true);
			break;
		}
		case operation_t::EXIT:
		{
			request.get_exact<>(true);
			break;
		}
		case operation_t::PING:
		{
			request.get_exact<>(end);
			break;
		}
		case operation_t::PIPE:
		{
			request.get_exact<>(end);
			break;
		}

		case operation_t::AUTH:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::ADD_USER:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::EDIT_USER:
		{
			request.get_exact<STR, CLIENT_AUTH>(end);
			break;
		}
		case operation_t::DEL_USER:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::CHANGE_PASSWORD:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::AUTH_LEVEL:
		{
			request.get_exact<>(end);
			break;
		}
		case operation_t::USERNAME:
		{
			request.get_exact<>(end);
			break;
		}

		case operation_t::SWITCH_DB:
		{
			request.get_exact<INT>(end);
			break;
		}
		case operation_t::PERSIST:
		{
			request.get_exact<>(end);
			break;
		}

		case operation_t::TRANSACT_BEGIN:
		{
			request.get_exact<>(end);
			break;
		}
		case operation_t::TRANSACT_COMMIT:
		{
			request.get_exact<>(end);
			break;
		}
		case operation_t::TRANSACT_DISCARD:
		{
			request.get_exact<>(end);
			break;
		}

		case operation_t::PUBLISH:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::SUBSCRIBE:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::SUBSCRIBED:
		{
			request.get_exact<>(end);
			break;
		}
		case operation_t::UNSUBSCRIBE:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::UNSUBSCRIBE_ALL:
		{
			request.get_exact<>(end);
			break;
		}

		case operation_t::DEL:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::TYPE:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::EXISTS:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::RESET:
		{
			request.get_exact<>(end);
			break;
		}
		case operation_t::KEYS:
		{
			request.get_exact<>(end);
			break;
		}
		case operation_t::COPY_TO:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::MOVE_TO:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::COPY_TO_DB:
		{
			request.get_exact<STR, INT>(end);
			break;
		}
		case operation_t::MOVE_TO_DB:
		{
			request.get_exact<STR, INT>(end);
			break;
		}

		case operation_t::SET_EXPIRY:
		{
			request.get_exact<STR, FLOAT>(end);
			break;
		}
		case operation_t::GET_EXPIRY:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::CLEAR_EXPIRY:
		{
			request.get_exact<STR>(end);
			break;
		}

		case operation_t::GET:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::STR_SET:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::INT_SET:
		{
			request.get_exact<STR, INT>(end);
			break;
		}
		case operation_t::FLOAT_SET:
		{
			request.get_exact<STR, FLOAT>(end);
			break;
		}
		case operation_t::INCR_INT:
		{
			request.get_exact<STR, INT>(end);
			break;
		}
		case operation_t::INCR_FLOAT:
		{
			request.get_exact<STR, FLOAT>(end);
			break;
		}
		case operation_t::STR_LEN:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::MANY_GET:
		{
			request.get_exact<ARR>(end);
			break;
		}

		case operation_t::LIST_LEN:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::LIST_GET:
		{
			request.get_exact<STR, INT>(end);
			break;
		}
		case operation_t::LIST_SET:
		{
			request.get_exact<STR, INT, STR>(end);
			break;
		}
		case operation_t::LIST_PUSH_LEFT:
		{
			request.get_exact<STR, LIST>(end);
			break;
		}
		case operation_t::LIST_PUSH_RIGHT:
		{
			request.get_exact<STR, LIST>(end);
			break;
		}
		case operation_t::LIST_POP_LEFT:
		{
			request.get_exact<STR, INT>(end);
			break;
		}
		case operation_t::LIST_POP_RIGHT:
		{
			request.get_exact<STR, INT>(end);
			break;
		}
		case operation_t::LIST_RANGE:
		{
			request.get_exact<STR, INT, INT>(end);
			break;
		}
		case operation_t::LIST_TRIM:
		{
			request.get_exact<STR, INT, INT>(end);
			break;
		}
		case operation_t::LIST_REMOVE:
		{
			request.get_exact<STR, STR, INT>(end);
			break;
		}

		case operation_t::SET_ADD:
		{
			request.get_exact<STR, SET>(end);
			break;
		}
		case operation_t::SET_ALL:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::SET_REMOVE:
		{
			request.get_exact<STR, INT>(end);
			break;
		}
		case operation_t::SET_DISCARD:
		{
			request.get_exact<STR, SET>(end);
			break;
		}
		case operation_t::SET_LEN:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::SET_CONTAINS:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::SET_MOVE:
		{
			request.get_exact<STR, STR, STR>(end);
			break;
		}
		case operation_t::SET_UNION:
		{
			request.get_exact<ARR>(end);
			break;
		}
		case operation_t::SET_UNION_INTO:
		{
			request.get_exact<STR, ARR>(end);
			break;
		}
		case operation_t::SET_UNION_LEN:
		{
			request.get_exact<ARR>(end);
			break;
		}
		case operation_t::SET_INTERSECT:
		{
			request.get_exact<ARR>(end);
			break;
		}
		case operation_t::SET_INTERSECT_INTO:
		{
			request.get_exact<STR, ARR>(end);
			break;
		}
		case operation_t::SET_INTERSECT_LEN:
		{
			request.get_exact<ARR>(end);
			break;
		}
		case operation_t::SET_DIFF:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::SET_DIFF_INTO:
		{
			request.get_exact<STR, STR, STR>(end);
			break;
		}
		case operation_t::SET_DIFF_LEN:
		{
			request.get_exact<STR, STR>(end);
			break;
		}

		case operation_t::HASH_SET:
		{
			request.get_exact<STR, HASH>(end);
			break;
		}
		case operation_t::HASH_ALL:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::HASH_GET:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::HASH_CONTAINS:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::HASH_LEN:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::HASH_KEY_LEN:
		{
			request.get_exact<STR, STR>(end);
			break;
		}
		case operation_t::HASH_REMOVE:
		{
			request.get_exact<STR, ARR>(end);
			break;
		}
		case operation_t::HASH_KEYS:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::HASH_VALUES:
		{
			request.get_exact<STR>(end);
			break;
		}
		case operation_t::HASH_UPDATE:
		{
			request.get_exact<STR, HASH>(end);
			break;
		}
		case operation_t::HASH_MANY_GET:
		{
			request.get_exact<STR, ARR>(end);
			break;
		}

		case operation_t::CLUSTER_JOIN:
		{
			request.get_exact<STR, INT>(end);
			break;
		}
	}
}

} // namespace vanity

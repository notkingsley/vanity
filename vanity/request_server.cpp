//
// Created by kingsli on 9/14/23.
//

#include "request_server.h"

namespace vanity {

void RequestServer::handle(const std::string& msg, Client& client) {
	Request request{msg};
	do_handle(client, request, true, false);
}

bool RequestServer::do_handle(Client &client, Request& request, bool end, bool strict) {
	try
	{
		return dispatch_request(client, request, end, strict);
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

void RequestServer::do_handle_many(Client &client, Request &request, size_t len) {
	if (len == 0)
		return;

	for (size_t i = 0; i < len - 1; ++i)
		do_handle(client, request, false, true);
	do_handle(client, request, true, false);
}

bool RequestServer::dispatch_request(Client &client, Request& request, bool end, bool strict) {
	auto state = session_state(client);

	switch (behaviour(request.peek_operation(), state)) {
		case behaviour_t::DEFAULT: {
			return dispatch_normal_request(client, request, end, strict);
		}
		case behaviour_t::NOT_PERMITTED: {
			if (strict)
				dry_dispatch_op(request.get_operation(), request, end);
			send(client, bad_state());
			return false;
		}
		case behaviour_t::CONTEXTUAL: {
			switch (state) {
				case conn_state::NORMAL:
					return dispatch_normal_request(client, request, end, strict);
				case conn_state::TRANSACTION:
					return dispatch_transaction_request(client, request, end, strict);
				default:
					throw std::runtime_error("invalid state");
			}
		}
		default:
			throw std::runtime_error("invalid behaviour");
	}
}

bool RequestServer::dispatch_normal_request(Client &client, Request& request, bool end, bool strict) {
	operation_t op = request.get_operation();

	if (not client.has_perm(op)){
		if (strict)
			dry_dispatch_op(op, request, end);

		send(client, denied());
		return false;
	}

	dispatch_op(client, op, request, end);
	return true;
}

void RequestServer::dispatch_op(Client &client, operation_t op, Request& request, bool end) {
	using object_t::STR, object_t::INT, object_t::FLOAT, object_t::ARR, object_t::LIST, object_t::SET, object_t::HASH;
	switch (op) {
		case operation_t::TERMINATE:
		{
			request.ensure_end();
			request_terminate(client);
			break;
		}
		case operation_t::PING:
		{
			request.ensure_end_if(end);
			request_ping(client);
			break;
		}
		case operation_t::EXIT:
		{
			request.ensure_end();
			request_exit(client);
			break;
		}
		case operation_t::PIPE:
		{
			request_pipe(client, request);
			request.ensure_end_if(end);
			break;
		}

		case operation_t::AUTH:
		{
			auto [username, password] = request.get_exact<STR, STR>(end);
			request_auth(client, username, password);
			break;
		}
		case operation_t::ADD_USER:
		{
			auto [username, password] = request.get_exact<STR, STR>(end);
			request_add_user(client, username, password);
			break;
		}
		case operation_t::EDIT_USER:
		{
			auto username = request.get<STR>();
			auto auth = request.get_client_auth();
			request.ensure_end_if(end);
			request_edit_user(client, username, auth);
			break;
		}
		case operation_t::DEL_USER:
		{
			request_del_user(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::CHANGE_PASSWORD:
		{
			request_change_password(client, request.get_exact<STR>(end));
			break;
		}

		case operation_t::SWITCH_DB:
		{
			request_switch_db(client, request.get_exact<INT>(end));
			break;
		}
		case operation_t::PERSIST:
		{
			request.ensure_end_if(end);
			request_persist(client);
			break;
		}

		case operation_t::TRANSACT_BEGIN:
		{
			request.ensure_end_if(end);
			request_transact_begin(client);
			break;
		}
		case operation_t::TRANSACT_COMMIT:
		{
			request.ensure_end_if(end);
			request_transact_commit(client);
			break;
		}
		case operation_t::TRANSACT_DISCARD:
		{
			request.ensure_end_if(end);
			request_transact_discard(client);
			break;
		}

		case operation_t::PUBLISH:
		{
			auto [channel, message] = request.get_exact<STR, STR>(end);
			request_publish(client, channel, message);
			break;
		}
		case operation_t::SUBSCRIBE:
		{
			request_subscribe(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::UNSUBSCRIBE:
		{
			request_unsubscribe(client, request.get_exact<STR>(end));
			break;
		}

		case operation_t::DEL:
		{
			request_del(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::TYPE:
		{
			request_type(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::EXISTS:
		{
			request_exists(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::RESET:
		{
			request.ensure_end_if(end);
			request_reset(client);
			break;
		}
		case operation_t::KEYS:
		{
			request.ensure_end_if(end);
			request_keys(client);
			break;
		}
		case operation_t::COPY_TO:
		{
			auto [from, to] = request.get_exact<STR, STR>(end);
			request_copy_to(client, from, to);
			break;
		}
		case operation_t::MOVE_TO:
		{
			auto [from, to] = request.get_exact<STR, STR>(end);
			request_move_to(client, from, to);
			break;
		}
		case operation_t::COPY_TO_DB:
		{
			auto [from, to] = request.get_exact<STR, INT>(end);
			request_copy_to_db(client, from, to);
			break;
		}
		case operation_t::MOVE_TO_DB:
		{
			auto [from, to] = request.get_exact<STR, INT>(end);
			request_move_to_db(client, from, to);
			break;
		}

		case operation_t::SET_EXPIRY:
		{
			auto [key, expiry] = request.get_exact<STR, FLOAT>(end);
			request_set_expiry(client, key, expiry);
			break;
		}
		case operation_t::GET_EXPIRY:
		{
			request_get_expiry(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::CLEAR_EXPIRY:
		{
			request_clear_expiry(client, request.get_exact<STR>(end));
			break;
		}

		case operation_t::GET:
		{
			request_get(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::STR_SET:
		{
			auto [key, value] = request.get_exact<STR, STR>(end);
			request_str_set(client, key, value);
			break;
		}
		case operation_t::INT_SET:
		{
			auto [key, value] = request.get_exact<STR, INT>(end);
			request_int_set(client, key, value);
			break;
		}
		case operation_t::FLOAT_SET:
		{
			auto [key, value] = request.get_exact<STR, FLOAT>(end);
			request_float_set(client, key, value);
			break;
		}
		case operation_t::INCR_INT:
		{
			auto [key, value] = request.get_exact<STR, INT>(end);
			request_incr_int(client, key, value);
			break;
		}
		case operation_t::INCR_FLOAT:
		{
			auto [key, value] = request.get_exact<STR, FLOAT>(end);
			request_incr_float(client, key, value);
			break;
		}
		case operation_t::STR_LEN:
		{
			request_str_len(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::MANY_GET:
		{
			request_many_get(client, request.get_exact<ARR>(end));
			break;
		}

		case operation_t::LIST_LEN:
		{
			request_list_len(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::LIST_GET:
		{
			auto [key, index] = request.get_exact<STR, INT>(end);
			request_list_get(client, key, index);
			break;
		}
		case operation_t::LIST_SET:
		{
			auto [key, index, value] = request.get_exact<STR, INT, STR>(end);
			request_list_set(client, key, index, value);
			break;
		}
		case operation_t::LIST_PUSH_LEFT:
		{
			auto [key, value] = request.get_exact<STR, LIST>(end);
			request_list_push_left(client, key, std::move(value));
			break;
		}
		case operation_t::LIST_PUSH_RIGHT:
		{
			auto [key, value] = request.get_exact<STR, LIST>(end);
			request_list_push_right(client, key, std::move(value));
			break;
		}
		case operation_t::LIST_POP_LEFT:
		{
			auto [key, count] = request.get_exact<STR, INT>(end);
			request_list_pop_left(client, key, count);
			break;
		}
		case operation_t::LIST_POP_RIGHT:
		{
			auto [key, count] = request.get_exact<STR, INT>(end);
			request_list_pop_right(client, key, count);
			break;
		}
		case operation_t::LIST_RANGE:
		{
			auto [key, start, stop] = request.get_exact<STR, INT, INT>(end);
			request_list_range(client, key, start, stop);
			break;
		}
		case operation_t::LIST_TRIM:
		{
			auto [key, start, stop] = request.get_exact<STR, INT, INT>(end);
			request_list_trim(client, key, start, stop);
			break;
		}
		case operation_t::LIST_REMOVE:
		{
			auto [key, value, count] = request.get_exact<STR, STR, INT>(end);
			request_list_remove(client, key, value, count);
			break;
		}

		case operation_t::SET_ADD:
		{
			auto [key, values] = request.get_exact<STR, SET>(end);
			request_set_add(client, key, std::move(values));
			break;
		}
		case operation_t::SET_ALL:
		{
			request_set_all(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::SET_REMOVE:
		{
			auto [key, count] = request.get_exact<STR, INT>(end);
			request_set_remove(client, key, count);
			break;
		}
		case operation_t::SET_DISCARD:
		{
			auto [key, values] = request.get_exact<STR, SET>(end);
			request_set_discard(client, key, std::move(values));
			break;
		}
		case operation_t::SET_LEN:
		{
			request_set_len(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::SET_CONTAINS:
		{
			auto [key, value] = request.get_exact<STR, STR>(end);
			request_set_contains(client, key, value);
			break;
		}
		case operation_t::SET_MOVE:
		{
			auto [source, dest, value] = request.get_exact<STR, STR, STR>(end);
			request_set_move(client, source, dest, value);
			break;
		}
		case operation_t::SET_UNION:
		{
			request_set_union(client, request.get_exact<ARR>(end));
			break;
		}
		case operation_t::SET_UNION_INTO:
		{
			auto [dest, keys] = request.get_exact<STR, ARR>(end);
			request_set_union_into(client, dest, keys);
			break;
		}
		case operation_t::SET_UNION_LEN:
		{
			request_set_union_len(client, request.get_exact<ARR>(end));
			break;
		}
		case operation_t::SET_INTERSECT:
		{
			request_set_intersection(client, request.get_exact<ARR>(end));
			break;
		}
		case operation_t::SET_INTERSECT_INTO:
		{
			auto [dest, keys] = request.get_exact<STR, ARR>(end);
			request_set_intersection_into(client, dest, keys);
			break;
		}
		case operation_t::SET_INTERSECT_LEN:
		{
			request_set_intersection_len(client, request.get_exact<ARR>(end));
			break;
		}
		case operation_t::SET_DIFF:
		{
			auto [key1, key2] = request.get_exact<STR, STR>(end);
			request_set_difference(client, key1, key2);
			break;
		}
		case operation_t::SET_DIFF_INTO:
		{
			auto [dest, key1, key2] = request.get_exact<STR, STR, STR>(end);
			request_set_difference_into(client, dest, key1, key2);
			break;
		}
		case operation_t::SET_DIFF_LEN:
		{
			auto [key1, key2] = request.get_exact<STR, STR>(end);
			request_set_difference_len(client, key1, key2);
			break;
		}

		case operation_t::HASH_SET:
		{
			auto [key, hash] = request.get_exact<STR, HASH>(end);
			request_hash_set(client, key, std::move(hash));
			break;
		}
		case operation_t::HASH_ALL:
		{
			request_hash_all(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::HASH_GET:
		{
			auto [key, hash_key] = request.get_exact<STR, STR>(end);
			request_hash_get(client, key, hash_key);
			break;
		}
		case operation_t::HASH_CONTAINS:
		{
			auto [key, hash_key] = request.get_exact<STR, STR>(end);
			request_hash_contains(client, key, hash_key);
			break;
		}
		case operation_t::HASH_LEN:
		{
			request_hash_len(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::HASH_KEY_LEN:
		{
			auto [key, hash_key] = request.get_exact<STR, STR>(end);
			request_hash_key_len(client, key, hash_key);
			break;
		}
		case operation_t::HASH_REMOVE:
		{
			auto [key, hash_key] = request.get_exact<STR, ARR>(end);
			request_hash_remove(client, key, hash_key);
			break;
		}
		case operation_t::HASH_KEYS:
		{
			request_hash_keys(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::HASH_VALUES:
		{
			request_hash_values(client, request.get_exact<STR>(end));
			break;
		}
		case operation_t::HASH_UPDATE:
		{
			auto [key, hash] = request.get_exact<STR, HASH>(end);
			request_hash_update(client, key, std::move(hash));
			break;
		}
		case operation_t::HASH_MANY_GET:
		{
			auto [key, hash_keys] = request.get_exact<STR, ARR>(end);
			request_hash_many_get(client, key, hash_keys);
			break;
		}
	}
}

void RequestServer::dry_dispatch_op(operation_t op, Request& request, bool end) {
	using object_t::STR, object_t::INT, object_t::FLOAT, object_t::ARR, object_t::LIST, object_t::SET, object_t::HASH;
	switch (op) {
		case operation_t::TERMINATE:
		{
			request.ensure_end();
			break;
		}
		case operation_t::PING:
		{
			request.ensure_end_if(end);
			break;
		}
		case operation_t::EXIT:
		{
			request.ensure_end();
			break;
		}
		case operation_t::PIPE:
		{
			request.ensure_end_if(end);
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
			request.get<STR>();
			request.get_client_auth();
			request.ensure_end_if(end);
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

		case operation_t::SWITCH_DB:
		{
			request.get_exact<INT>(end);
			break;
		}
		case operation_t::PERSIST:
		{
			request.ensure_end_if(end);
			break;
		}

		case operation_t::TRANSACT_BEGIN:
		{
			request.ensure_end_if(end);
			break;
		}
		case operation_t::TRANSACT_COMMIT:
		{
			request.ensure_end_if(end);
			break;
		}
		case operation_t::TRANSACT_DISCARD:
		{
			request.ensure_end_if(end);
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
		case operation_t::UNSUBSCRIBE:
		{
			request.get_exact<STR>(end);
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
			request.ensure_end_if(end);
			break;
		}
		case operation_t::KEYS:
		{
			request.ensure_end_if(end);
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
	}
}

} // namespace vanity

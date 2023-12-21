//
// Created by kingsli on 9/14/23.
//

#include "extract.h"
#include "request_server.h"

namespace vanity {

void RequestServer::handle(const std::string& msg, Client& client) {
	Request request{msg};
	do_handle(client, request, true, false);
}

bool RequestServer::do_handle(Client &client, Request& request, bool end, bool strict) {
	try
	{
		return do_handle_inner(client, request, end, strict);
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

bool RequestServer::do_handle_inner(Client &client, Request& request, bool end, bool strict) {
	operation_t op = extract_operation(request);

	if (not client.has_perm(op)){
		if (strict)
			dry_dispatch_op(op, request, end);

		send(client, denied());
		return false;
	}

	dispatch_op(client, op, request, end);
	return true;
}

void RequestServer::dispatch_op(Client &client, operation_t op, Request& request, bool expect_end) {
	using object_t::STR, object_t::INT, object_t::FLOAT, object_t::ARR, object_t::LIST, object_t::SET, object_t::HASH;
	switch (op) {
		case operation_t::TERMINATE:
		{
			ensure_end(request);
			request_terminate(client);
			break;
		}
		case operation_t::PING:
		{
			request_ping(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::EXIT:
		{
			ensure_end(request);
			request_exit(client);
			break;
		}
		case operation_t::PIPE:
		{
			request_pipe(client, request);
			ensure_end_if(request, expect_end);
			break;
		}

		case operation_t::AUTH:
		{
			auto [username, password] = extract_exact<STR, STR>(request, expect_end);
			request_auth(client, username, password);
			break;
		}
		case operation_t::ADD_USER:
		{
			auto [username, password] = extract_exact<STR, STR>(request, expect_end);
			request_add_user(client, username, password);
			break;
		}
		case operation_t::EDIT_USER:
		{
			auto username = extract<STR>(request);
			auto auth = extract_client_auth(request);
			ensure_end_if(request, expect_end);
			request_edit_user(client, username, auth);
			break;
		}
		case operation_t::DEL_USER:
		{
			request_del_user(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::CHANGE_PASSWORD:
		{
			request_change_password(client, extract_exact<STR>(request, expect_end));
			break;
		}

		case operation_t::SWITCH_DB:
		{
			request_switch_db(client, extract_exact<INT>(request, expect_end));
			break;
		}
		case operation_t::PERSIST:
		{
			ensure_end_if(request, expect_end);
			request_persist(client);
			break;
		}

		case operation_t::TRANSACT_BEGIN:
		{
			ensure_end_if(request, expect_end);
			request_transact_begin(client);
			break;
		}
		case operation_t::TRANSACT_COMMIT:
		{
			ensure_end_if(request, expect_end);
			request_transact_commit(client);
			break;
		}
		case operation_t::TRANSACT_DISCARD:
		{
			ensure_end_if(request, expect_end);
			request_transact_discard(client);
			break;
		}

		case operation_t::DEL:
		{
			request_del(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::TYPE:
		{
			request_type(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::EXISTS:
		{
			request_exists(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::RESET:
		{
			ensure_end_if(request, expect_end);
			request_reset(client);
			break;
		}
		case operation_t::KEYS:
		{
			ensure_end_if(request, expect_end);
			request_keys(client);
			break;
		}
		case operation_t::COPY_TO:
		{
			auto [from, to] = extract_exact<STR, STR>(request, expect_end);
			request_copy_to(client, from, to);
			break;
		}
		case operation_t::MOVE_TO:
		{
			auto [from, to] = extract_exact<STR, STR>(request, expect_end);
			request_move_to(client, from, to);
			break;
		}
		case operation_t::COPY_TO_DB:
		{
			auto [from, to] = extract_exact<STR, INT>(request, expect_end);
			request_copy_to_db(client, from, to);
			break;
		}
		case operation_t::MOVE_TO_DB:
		{
			auto [from, to] = extract_exact<STR, INT>(request, expect_end);
			request_move_to_db(client, from, to);
			break;
		}

		case operation_t::SET_EXPIRY:
		{
			auto [key, expiry] = extract_exact<STR, FLOAT>(request, expect_end);
			request_set_expiry(client, key, expiry);
			break;
		}
		case operation_t::GET_EXPIRY:
		{
			request_get_expiry(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::CLEAR_EXPIRY:
		{
			request_clear_expiry(client, extract_exact<STR>(request, expect_end));
			break;
		}

		case operation_t::GET:
		{
			request_get(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::STR_SET:
		{
			auto [key, value] = extract_exact<STR, STR>(request, expect_end);
			request_str_set(client, key, value);
			break;
		}
		case operation_t::INT_SET:
		{
			auto [key, value] = extract_exact<STR, INT>(request, expect_end);
			request_int_set(client, key, value);
			break;
		}
		case operation_t::FLOAT_SET:
		{
			auto [key, value] = extract_exact<STR, FLOAT>(request, expect_end);
			request_float_set(client, key, value);
			break;
		}
		case operation_t::INCR_INT:
		{
			auto [key, value] = extract_exact<STR, INT>(request, expect_end);
			request_incr_int(client, key, value);
			break;
		}
		case operation_t::INCR_FLOAT:
		{
			auto [key, value] = extract_exact<STR, FLOAT>(request, expect_end);
			request_incr_float(client, key, value);
			break;
		}
		case operation_t::STR_LEN:
		{
			request_str_len(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::MANY_GET:
		{
			request_many_get(client, extract_exact<ARR>(request, expect_end));
			break;
		}

		case operation_t::LIST_LEN:
		{
			request_list_len(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::LIST_GET:
		{
			auto [key, index] = extract_exact<STR, INT>(request, expect_end);
			request_list_get(client, key, index);
			break;
		}
		case operation_t::LIST_SET:
		{
			auto [key, index, value] = extract_exact<STR, INT, STR>(request, expect_end);
			request_list_set(client, key, index, value);
			break;
		}
		case operation_t::LIST_PUSH_LEFT:
		{
			auto [key, value] = extract_exact<STR, LIST>(request, expect_end);
			request_list_push_left(client, key, std::move(value));
			break;
		}
		case operation_t::LIST_PUSH_RIGHT:
		{
			auto [key, value] = extract_exact<STR, LIST>(request, expect_end);
			request_list_push_right(client, key, std::move(value));
			break;
		}
		case operation_t::LIST_POP_LEFT:
		{
			auto [key, count] = extract_exact<STR, INT>(request, expect_end);
			request_list_pop_left(client, key, count);
			break;
		}
		case operation_t::LIST_POP_RIGHT:
		{
			auto [key, count] = extract_exact<STR, INT>(request, expect_end);
			request_list_pop_right(client, key, count);
			break;
		}
		case operation_t::LIST_RANGE:
		{
			auto [key, start, stop] = extract_exact<STR, INT, INT>(request, expect_end);
			request_list_range(client, key, start, stop);
			break;
		}
		case operation_t::LIST_TRIM:
		{
			auto [key, start, stop] = extract_exact<STR, INT, INT>(request, expect_end);
			request_list_trim(client, key, start, stop);
			break;
		}
		case operation_t::LIST_REMOVE:
		{
			auto [key, value, count] = extract_exact<STR, STR, INT>(request, expect_end);
			request_list_remove(client, key, value, count);
			break;
		}

		case operation_t::SET_ADD:
		{
			auto [key, values] = extract_exact<STR, SET>(request, expect_end);
			request_set_add(client, key, std::move(values));
			break;
		}
		case operation_t::SET_ALL:
		{
			request_set_all(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::SET_REMOVE:
		{
			auto [key, count] = extract_exact<STR, INT>(request, expect_end);
			request_set_remove(client, key, count);
			break;
		}
		case operation_t::SET_DISCARD:
		{
			auto [key, values] = extract_exact<STR, SET>(request, expect_end);
			request_set_discard(client, key, std::move(values));
			break;
		}
		case operation_t::SET_LEN:
		{
			request_set_len(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::SET_CONTAINS:
		{
			auto [key, value] = extract_exact<STR, STR>(request, expect_end);
			request_set_contains(client, key, value);
			break;
		}
		case operation_t::SET_MOVE:
		{
			auto [source, dest, value] = extract_exact<STR, STR, STR>(request, expect_end);
			request_set_move(client, source, dest, value);
			break;
		}
		case operation_t::SET_UNION:
		{
			request_set_union(client, extract_exact<ARR>(request, expect_end));
			break;
		}
		case operation_t::SET_UNION_INTO:
		{
			auto [dest, keys] = extract_exact<STR, ARR>(request, expect_end);
			request_set_union_into(client, dest, keys);
			break;
		}
		case operation_t::SET_UNION_LEN:
		{
			request_set_union_len(client, extract_exact<ARR>(request, expect_end));
			break;
		}
		case operation_t::SET_INTERSECT:
		{
			request_set_intersection(client, extract_exact<ARR>(request, expect_end));
			break;
		}
		case operation_t::SET_INTERSECT_INTO:
		{
			auto [dest, keys] = extract_exact<STR, ARR>(request, expect_end);
			request_set_intersection_into(client, dest, keys);
			break;
		}
		case operation_t::SET_INTERSECT_LEN:
		{
			request_set_intersection_len(client, extract_exact<ARR>(request, expect_end));
			break;
		}
		case operation_t::SET_DIFF:
		{
			auto [key1, key2] = extract_exact<STR, STR>(request, expect_end);
			request_set_difference(client, key1, key2);
			break;
		}
		case operation_t::SET_DIFF_INTO:
		{
			auto [dest, key1, key2] = extract_exact<STR, STR, STR>(request, expect_end);
			request_set_difference_into(client, dest, key1, key2);
			break;
		}
		case operation_t::SET_DIFF_LEN:
		{
			auto [key1, key2] = extract_exact<STR, STR>(request, expect_end);
			request_set_difference_len(client, key1, key2);
			break;
		}

		case operation_t::HASH_SET:
		{
			auto [key, hash] = extract_exact<STR, HASH>(request, expect_end);
			request_hash_set(client, key, std::move(hash));
			break;
		}
		case operation_t::HASH_ALL:
		{
			request_hash_all(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::HASH_GET:
		{
			auto [key, hash_key] = extract_exact<STR, STR>(request, expect_end);
			request_hash_get(client, key, hash_key);
			break;
		}
		case operation_t::HASH_CONTAINS:
		{
			auto [key, hash_key] = extract_exact<STR, STR>(request, expect_end);
			request_hash_contains(client, key, hash_key);
			break;
		}
		case operation_t::HASH_LEN:
		{
			request_hash_len(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::HASH_KEY_LEN:
		{
			auto [key, hash_key] = extract_exact<STR, STR>(request, expect_end);
			request_hash_key_len(client, key, hash_key);
			break;
		}
		case operation_t::HASH_REMOVE:
		{
			auto [key, hash_key] = extract_exact<STR, ARR>(request, expect_end);
			request_hash_remove(client, key, hash_key);
			break;
		}
		case operation_t::HASH_KEYS:
		{
			request_hash_keys(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::HASH_VALUES:
		{
			request_hash_values(client, extract_exact<STR>(request, expect_end));
			break;
		}
		case operation_t::HASH_UPDATE:
		{
			auto [key, hash] = extract_exact<STR, HASH>(request, expect_end);
			request_hash_update(client, key, std::move(hash));
			break;
		}
		case operation_t::HASH_MANY_GET:
		{
			auto [key, hash_keys] = extract_exact<STR, ARR>(request, expect_end);
			request_hash_many_get(client, key, hash_keys);
			break;
		}
	}
}

void RequestServer::dry_dispatch_op(operation_t op, Request& request, bool expect_end) {
	using object_t::STR, object_t::INT, object_t::FLOAT, object_t::ARR, object_t::LIST, object_t::SET, object_t::HASH;
	switch (op) {
		case operation_t::TERMINATE:
		{
			ensure_end(request);
			break;
		}
		case operation_t::PING:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::EXIT:
		{
			ensure_end(request);
			break;
		}
		case operation_t::PIPE:
		{
			ensure_end_if(request, expect_end);
			break;
		}

		case operation_t::AUTH:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}
		case operation_t::ADD_USER:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}
		case operation_t::EDIT_USER:
		{
			extract<STR>(request);
			extract_client_auth(request);
			ensure_end_if(request, expect_end);
			break;
		}
		case operation_t::DEL_USER:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::CHANGE_PASSWORD:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}

		case operation_t::SWITCH_DB:
		{
			extract_exact<INT>(request, expect_end);
			break;
		}
		case operation_t::PERSIST:
		{
			ensure_end_if(request, expect_end);
			break;
		}

		case operation_t::TRANSACT_BEGIN:
		{
			ensure_end_if(request, expect_end);
			break;
		}
		case operation_t::TRANSACT_COMMIT:
		{
			ensure_end_if(request, expect_end);
			break;
		}
		case operation_t::TRANSACT_DISCARD:
		{
			ensure_end_if(request, expect_end);
			break;
		}

		case operation_t::DEL:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::TYPE:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::EXISTS:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::RESET:
		{
			ensure_end_if(request, expect_end);
			break;
		}
		case operation_t::KEYS:
		{
			ensure_end_if(request, expect_end);
			break;
		}
		case operation_t::COPY_TO:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}
		case operation_t::MOVE_TO:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}
		case operation_t::COPY_TO_DB:
		{
			extract_exact<STR, INT>(request, expect_end);
			break;
		}
		case operation_t::MOVE_TO_DB:
		{
			extract_exact<STR, INT>(request, expect_end);
			break;
		}

		case operation_t::SET_EXPIRY:
		{
			extract_exact<STR, FLOAT>(request, expect_end);
			break;
		}
		case operation_t::GET_EXPIRY:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::CLEAR_EXPIRY:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}

		case operation_t::GET:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::STR_SET:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}
		case operation_t::INT_SET:
		{
			extract_exact<STR, INT>(request, expect_end);
			break;
		}
		case operation_t::FLOAT_SET:
		{
			extract_exact<STR, FLOAT>(request, expect_end);
			break;
		}
		case operation_t::INCR_INT:
		{
			extract_exact<STR, INT>(request, expect_end);
			break;
		}
		case operation_t::INCR_FLOAT:
		{
			extract_exact<STR, FLOAT>(request, expect_end);
			break;
		}
		case operation_t::STR_LEN:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::MANY_GET:
		{
			extract_exact<ARR>(request, expect_end);
			break;
		}

		case operation_t::LIST_LEN:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::LIST_GET:
		{
			extract_exact<STR, INT>(request, expect_end);
			break;
		}
		case operation_t::LIST_SET:
		{
			extract_exact<STR, INT, STR>(request, expect_end);
			break;
		}
		case operation_t::LIST_PUSH_LEFT:
		{
			extract_exact<STR, LIST>(request, expect_end);
			break;
		}
		case operation_t::LIST_PUSH_RIGHT:
		{
			extract_exact<STR, LIST>(request, expect_end);
			break;
		}
		case operation_t::LIST_POP_LEFT:
		{
			extract_exact<STR, INT>(request, expect_end);
			break;
		}
		case operation_t::LIST_POP_RIGHT:
		{
			extract_exact<STR, INT>(request, expect_end);
			break;
		}
		case operation_t::LIST_RANGE:
		{
			extract_exact<STR, INT, INT>(request, expect_end);
			break;
		}
		case operation_t::LIST_TRIM:
		{
			extract_exact<STR, INT, INT>(request, expect_end);
			break;
		}
		case operation_t::LIST_REMOVE:
		{
			extract_exact<STR, STR, INT>(request, expect_end);
			break;
		}

		case operation_t::SET_ADD:
		{
			extract_exact<STR, SET>(request, expect_end);
			break;
		}
		case operation_t::SET_ALL:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::SET_REMOVE:
		{
			extract_exact<STR, INT>(request, expect_end);
			break;
		}
		case operation_t::SET_DISCARD:
		{
			extract_exact<STR, SET>(request, expect_end);
			break;
		}
		case operation_t::SET_LEN:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::SET_CONTAINS:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}
		case operation_t::SET_MOVE:
		{
			extract_exact<STR, STR, STR>(request, expect_end);
			break;
		}
		case operation_t::SET_UNION:
		{
			extract_exact<ARR>(request, expect_end);
			break;
		}
		case operation_t::SET_UNION_INTO:
		{
			extract_exact<STR, ARR>(request, expect_end);
			break;
		}
		case operation_t::SET_UNION_LEN:
		{
			extract_exact<ARR>(request, expect_end);
			break;
		}
		case operation_t::SET_INTERSECT:
		{
			extract_exact<ARR>(request, expect_end);
			break;
		}
		case operation_t::SET_INTERSECT_INTO:
		{
			extract_exact<STR, ARR>(request, expect_end);
			break;
		}
		case operation_t::SET_INTERSECT_LEN:
		{
			extract_exact<ARR>(request, expect_end);
			break;
		}
		case operation_t::SET_DIFF:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}
		case operation_t::SET_DIFF_INTO:
		{
			extract_exact<STR, STR, STR>(request, expect_end);
			break;
		}
		case operation_t::SET_DIFF_LEN:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}

		case operation_t::HASH_SET:
		{
			extract_exact<STR, HASH>(request, expect_end);
			break;
		}
		case operation_t::HASH_ALL:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::HASH_GET:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}
		case operation_t::HASH_CONTAINS:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}
		case operation_t::HASH_LEN:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::HASH_KEY_LEN:
		{
			extract_exact<STR, STR>(request, expect_end);
			break;
		}
		case operation_t::HASH_REMOVE:
		{
			extract_exact<STR, ARR>(request, expect_end);
			break;
		}
		case operation_t::HASH_KEYS:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::HASH_VALUES:
		{
			extract_exact<STR>(request, expect_end);
			break;
		}
		case operation_t::HASH_UPDATE:
		{
			extract_exact<STR, HASH>(request, expect_end);
			break;
		}
		case operation_t::HASH_MANY_GET:
		{
			extract_exact<STR, ARR>(request, expect_end);
			break;
		}
	}
}

} // namespace vanity

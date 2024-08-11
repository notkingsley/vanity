//
// Created by kingsli on 6/30/24.
//

#ifndef VANITY_DB_OPERATIONS_H
#define VANITY_DB_OPERATIONS_H

#include <cstdlib>
#include <stdexcept>


namespace vanity::db {

/*
 * All the operations that can be performed on a database
 */
enum class db_op_t : uint {
	persist,
	reset,
	has,
	del,
	get,
	type,
	keys,
	copy_to,
	move_to,
	copy_to_db,
	move_to_db,

	clear_expiry,
	set_expiry,
	get_expiry,
	clear_all_expiry,
	shallow_purge,
	deep_purge,

	str_set,
	int_set,
	float_set,
	incr_int,
	incr_float,
	str_len,
	many_get,
	many_set,

	list_len,
	list_get,
	list_set,
	list_push_left,
	list_push_right,
	list_pop_left,
	list_pop_right,
	list_range,
	list_trim,
	list_remove,

	set_add,
	set_all,
	set_remove,
	set_discard,
	set_len,
	set_contains,
	set_move,
	set_union,
	set_union_into,
	set_union_len,
	set_intersection,
	set_intersection_into,
	set_intersection_len,
	set_difference,
	set_difference_into,
	set_difference_len,

	hash_set,
	hash_all,
	hash_get,
	hash_contains,
	hash_len,
	hash_key_len,
	hash_remove,
	hash_keys,
	hash_values,
	hash_update,
	hash_many_get,
};

// check if a db_op_t should be written to the WAL
inline bool should_wal(db_op_t op)
{
	switch (op) {
		case db_op_t::persist:
		case db_op_t::has:
		case db_op_t::get:
		case db_op_t::type:
		case db_op_t::keys:

		case db_op_t::get_expiry:
		case db_op_t::shallow_purge:
		case db_op_t::deep_purge:

		case db_op_t::str_len:
		case db_op_t::many_get:

		case db_op_t::list_len:
		case db_op_t::list_get:
		case db_op_t::list_range:

		case db_op_t::set_all:
		case db_op_t::set_len:
		case db_op_t::set_contains:
		case db_op_t::set_union:
		case db_op_t::set_union_len:
		case db_op_t::set_intersection:
		case db_op_t::set_intersection_len:
		case db_op_t::set_difference:
		case db_op_t::set_difference_len:

		case db_op_t::hash_all:
		case db_op_t::hash_get:
		case db_op_t::hash_contains:
		case db_op_t::hash_len:
		case db_op_t::hash_key_len:
		case db_op_t::hash_keys:
		case db_op_t::hash_values:
		case db_op_t::hash_many_get:
			return false;


		case db_op_t::reset:
		case db_op_t::del:
		case db_op_t::copy_to:
		case db_op_t::move_to:
		case db_op_t::copy_to_db:
		case db_op_t::move_to_db:

		case db_op_t::set_expiry:
		case db_op_t::clear_expiry:
		case db_op_t::clear_all_expiry:

		case db_op_t::str_set:
		case db_op_t::int_set:
		case db_op_t::float_set:
		case db_op_t::incr_int:
		case db_op_t::incr_float:
		case db_op_t::many_set:

		case db_op_t::list_set:
		case db_op_t::list_push_left:
		case db_op_t::list_push_right:
		case db_op_t::list_pop_left:
		case db_op_t::list_pop_right:
		case db_op_t::list_trim:
		case db_op_t::list_remove:

		case db_op_t::set_add:
		case db_op_t::set_remove:
		case db_op_t::set_discard:
		case db_op_t::set_move:
		case db_op_t::set_union_into:
		case db_op_t::set_intersection_into:
		case db_op_t::set_difference_into:

		case db_op_t::hash_set:
		case db_op_t::hash_remove:
		case db_op_t::hash_update:
			return true;

		default:
			throw std::runtime_error("unknown db_op_t");
	}
}

} // namespace vanity::db

#endif // VANITY_DB_OPERATIONS_H

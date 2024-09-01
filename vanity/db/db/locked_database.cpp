//
// Created by kingsli on 11/11/23.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#include "locked_database.h"

namespace vanity::db {

LockedDatabase::LockedDatabase(m_index_type index, logger_type &wal_logger)
	: Database{index}, m_wal_logger{wal_logger} {}

LockedDatabase::LockedDatabase(Database &&other, logger_type &wal_logger) noexcept
	: Database{std::move(other)}, m_wal_logger{wal_logger} {}

LockedDatabase &LockedDatabase::operator=(Database &&other) noexcept {
	Database::operator=(std::move(other));
	return *this;
}

LockedDatabase LockedDatabase::from(std::ifstream &in, logger_type &wal_logger) {
	return LockedDatabase{Database::from(in), wal_logger};
}


std::lock_guard<LockedDatabase::lock_type> LockedDatabase::lock() {
	return std::lock_guard{m_mutex};
}

auto LockedDatabase::mutex() -> lock_type & {
	return m_mutex;
}


template<typename ...Args>
inline void LockedDatabase::wal_log(trn_id_t trn_id, db_op_t op, const Args &... args) {
	if (should_wal(op))
		m_wal_logger.wal_db_op(m_index, trn_id, op, args...);
}


void LockedDatabase::wal_redo_db_op(db_op_t op, std::ifstream &in, get_db_func_t get_db) {
	serializer::ReadHandle reader{in};
	switch (op) {
		case db_op_t::has:
		case db_op_t::get:
		case db_op_t::type:
		case db_op_t::keys:

		case db_op_t::get_expiry:

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
			throw std::runtime_error("unexpected db_op_t");


		case db_op_t::reset: {
			Database::reset();
			break;
		}
		case db_op_t::del: {
			Database::del(reader.read<key_type>());
			break;
		}
		case db_op_t::copy_to: {
			auto [from, to] = reader.read<key_type, key_type>();
			Database::copy_to(from, to);
			break;
		}
		case db_op_t::move_to: {
			auto [from, to] = reader.read<key_type, key_type>();
			Database::move_to(from, to);
			break;
		}
		case db_op_t::copy_to_db: {
			auto [from, to] = reader.read<key_type, m_index_type>();
			 Database::copy_to_db(from, get_db(to));
			break;
		}
		case db_op_t::move_to_db: {
			auto [from, to] = reader.read<key_type, m_index_type>();
			 Database::move_to_db(from, get_db(to));
			break;
		}

		case db_op_t::set_expiry: {
			auto [key, expiry_time] = reader.read<key_type, time_t>();
			Database::set_expiry(key, expiry_time);
			break;
		}
		case db_op_t::clear_expiry: {
			Database::clear_expiry(reader.read<key_type>());
			break;
		}
		case db_op_t::clear_all_expiry: {
			Database::clear_all_expiry();
			break;
		}

		case db_op_t::str_set: {
			auto [key, value] = reader.read<key_type, std::string>();
			Database::str_set(key, value);
			break;
		}
		case db_op_t::int_set: {
			auto [key, value] = reader.read<key_type, int_t>();
			Database::int_set(key, value);
			break;
		}
		case db_op_t::float_set: {
			auto [key, value] = reader.read<key_type, float_t>();
			Database::float_set(key, value);
			break;
		}
		case db_op_t::incr_int: {
			auto [key, value] = reader.read<key_type, int_t>();
			Database::incr_int(key, value);
			break;
		}
		case db_op_t::incr_float: {
			auto [key, value] = reader.read<key_type, float_t>();
			Database::incr_float(key, value);
			break;
		}

		case db_op_t::list_set: {
			auto [key, index, value] = reader.read<key_type, int64_t, std::string>();
			Database::list_set(key, index, value);
			break;
		}
		case db_op_t::list_push_left: {
			auto [key, values] = reader.read<key_type, list_t>();
			Database::list_push_left(key, values);
			break;
		}
		case db_op_t::list_push_right: {
			auto [key, values] = reader.read<key_type, list_t>();
			Database::list_push_right(key, values);
			break;
		}
		case db_op_t::list_pop_left: {
			auto [key, n] = reader.read<key_type, int64_t>();
			Database::list_pop_left(key, n);
			break;
		}
		case db_op_t::list_pop_right: {
			auto [key, n] = reader.read<key_type, int64_t>();
			Database::list_pop_right(key, n);
			break;
		}
		case db_op_t::list_trim: {
			auto [key, start, end] = reader.read<key_type, int64_t, int64_t>();
			Database::list_trim(key, start, end);
			break;
		}
		case db_op_t::list_remove: {
			auto [key, element, count] = reader.read<key_type, std::string, int64_t>();
			Database::list_remove(key, element, count);
			break;
		}

		case db_op_t::set_add: {
			auto [key, values] = reader.read<key_type, set_t>();
			Database::set_add(key, values);
			break;
		}
		case db_op_t::set_remove: {
			auto [key, count] = reader.read<key_type, size_t>();
			Database::set_remove(key, count);
			break;
		}
		case db_op_t::set_discard: {
			auto [key, values] = reader.read<key_type, set_t>();
			Database::set_discard(key, values);
			break;
		}
		case db_op_t::set_move: {
			auto [source, dest, value] = reader.read<key_type, key_type, std::string>();
			Database::set_move(source, dest, value);
			break;
		}
		case db_op_t::set_union_into: {
			auto [dest, keys] = reader.read<key_type, std::vector<key_type>>();
			Database::set_union_into(dest, keys);
			break;
		}
		case db_op_t::set_intersection_into: {
			auto [dest, keys] = reader.read<key_type, std::vector<key_type>>();
			Database::set_intersection_into(dest, keys);
			break;
		}
		case db_op_t::set_difference_into: {
			auto [dest, key1, key2] = reader.read<key_type, key_type, key_type>();
			Database::set_difference_into(dest, key1, key2);
			break;
		}

		case db_op_t::hash_set: {
			auto [key, values] = reader.read<key_type, hash_t>();
			Database::hash_set(key, values);
			break;
		}
		case db_op_t::hash_remove: {
			auto [key, hash_keys] = reader.read<key_type, std::vector<string_t>>();
			Database::hash_remove(key, hash_keys);
			break;
		}
		case db_op_t::hash_update: {
			auto [key, values] = reader.read<key_type, hash_t>();
			Database::hash_update(key, values);
			break;
		}

		default:
			throw std::runtime_error("unknown db_op_t");
	}
}

void LockedDatabase::wal_redo_expiry(const key_type &key) {
	Database::force_expire(key);
}


void LockedDatabase::persist(std::ofstream &out) {
	std::lock_guard lock{m_mutex};
	Database::persist(out);
}

void LockedDatabase::reset(trn_id_t trn_id) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::reset);
	Database::reset();
}

bool LockedDatabase::has(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::has, key);
	return Database::has(key);
}

bool LockedDatabase::del(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::del, key);
	return Database::del(key);
}

std::optional<Database::data_type> LockedDatabase::get(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::get, key);
	return Database::get(key);
}

std::optional<int> LockedDatabase::type(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::type, key);
	return Database::type(key);
}

std::vector<Database::key_type> LockedDatabase::keys(trn_id_t trn_id) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::keys);
	return Database::keys();
}

bool LockedDatabase::copy_to(trn_id_t trn_id, const key_type &from, const key_type &to) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::copy_to, from, to);
	return Database::copy_to(from, to);
}

bool LockedDatabase::move_to(trn_id_t trn_id, const key_type &from, const key_type &to) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::move_to, from, to);
	return Database::move_to(from, to);
}

bool LockedDatabase::copy_to_db(trn_id_t trn_id, const key_type &from, LockedDatabase &to) {
	std::scoped_lock lock{m_mutex, to.m_mutex};
	wal_log(trn_id, db_op_t::copy_to_db, from, to.m_index);
	return Database::copy_to_db(from, to);
}

bool LockedDatabase::move_to_db(trn_id_t trn_id, const key_type &from, LockedDatabase &to) {
	std::scoped_lock lock{m_mutex, to.m_mutex};
	wal_log(trn_id, db_op_t::move_to_db, from, to.m_index);
	return Database::move_to_db(from, to);
}


void LockedDatabase::set_expiry(trn_id_t trn_id, const key_type &key, time_t expiry_time) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_expiry, key, expiry_time);
	Database::set_expiry(key, expiry_time);
}

std::optional<time_t> LockedDatabase::get_expiry(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::get_expiry, key);
	return Database::get_expiry(key);
}

void LockedDatabase::clear_expiry(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::clear_expiry, key);
	Database::clear_expiry(key);
}

void LockedDatabase::clear_all_expiry(trn_id_t trn_id) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::clear_all_expiry);
	Database::clear_all_expiry();
}

void LockedDatabase::shallow_purge() {
	std::lock_guard lock{m_mutex};
	Database::shallow_purge();
}

void LockedDatabase::deep_purge() {
	std::lock_guard lock{m_mutex};
	Database::deep_purge();
}

void LockedDatabase::expiry_enabled(bool enable) {
	std::lock_guard lock{m_mutex};
	Database::expiry_enabled(enable);
}

void LockedDatabase::pre_expire(const key_type &key) {
	m_wal_logger.wal_expiry(key, m_index);
}


void LockedDatabase::str_set(trn_id_t trn_id, const key_type &key, const std::string &value) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::str_set, key, value);
	Database::str_set(key, value);
}

void LockedDatabase::int_set(trn_id_t trn_id, const key_type &key, int_t value) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::int_set, key, value);
	Database::int_set(key, value);
}

void LockedDatabase::float_set(trn_id_t trn_id, const key_type &key, float_t value) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::float_set, key, value);
	Database::float_set(key, value);
}

std::optional<int_t> LockedDatabase::incr_int(trn_id_t trn_id, const key_type &key, int_t value) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::incr_int, key, value);
	return Database::incr_int(key, value);
}

std::optional<float_t> LockedDatabase::incr_float(trn_id_t trn_id, const key_type &key, float_t value) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::incr_float, key, value);
	return Database::incr_float(key, value);
}

std::optional<int_t> LockedDatabase::str_len(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::str_len, key);
	return Database::str_len(key);
}

std::vector<std::optional<Database::data_type>> LockedDatabase::many_get(trn_id_t trn_id, const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::many_get, keys);
	return Database::many_get(keys);
}


std::variant<size_t, ListErrorKind> LockedDatabase::list_len(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::list_len, key);
	return Database::list_len(key);
}

std::variant<std::string, ListErrorKind> LockedDatabase::list_get(trn_id_t trn_id, const key_type &key, int64_t index) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::list_get, key, index);
	return Database::list_get(key, index);
}

std::variant<bool, ListErrorKind>
LockedDatabase::list_set(trn_id_t trn_id, const key_type &key, int64_t index, const std::string &value) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::list_set, key, index, value);
	return Database::list_set(key, index, value);
}

std::variant<size_t, ListErrorKind> LockedDatabase::list_push_left(trn_id_t trn_id, const key_type &key, list_t values) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::list_push_left, key, values);
	return Database::list_push_left(key, std::move(values));
}

std::variant<size_t, ListErrorKind> LockedDatabase::list_push_right(trn_id_t trn_id, const key_type &key, list_t values) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::list_push_right, key, values);
	return Database::list_push_right(key, std::move(values));
}

std::variant<list_t, ListErrorKind> LockedDatabase::list_pop_left(trn_id_t trn_id, const key_type &key, int64_t n) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::list_pop_left, key, n);
	return Database::list_pop_left(key, n);
}

std::variant<list_t, ListErrorKind> LockedDatabase::list_pop_right(trn_id_t trn_id, const key_type &key, int64_t n) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::list_pop_right, key, n);
	return Database::list_pop_right(key, n);
}

std::variant<list_t, ListErrorKind>
LockedDatabase::list_range(trn_id_t trn_id, const key_type &key, int64_t start, int64_t end) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::list_range, key, start, end);
	return Database::list_range(key, start, end);
}

std::variant<size_t, ListErrorKind>
LockedDatabase::list_trim(trn_id_t trn_id, const key_type &key, int64_t start, int64_t end) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::list_trim, key, start, end);
	return Database::list_trim(key, start, end);

}

std::variant<size_t, ListErrorKind>
LockedDatabase::list_remove(trn_id_t trn_id, const key_type &key, const std::string &element, int64_t count) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::list_remove, key, element, count);
	return Database::list_remove(key, element, count);
}


std::optional<size_t> LockedDatabase::set_add(trn_id_t trn_id, const key_type &key, set_t values) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_add, key, values);
	return Database::set_add(key, std::move(values));
}

std::optional<set_t> LockedDatabase::set_all(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_all, key);
	return Database::set_all(key);
}

std::optional<set_t> LockedDatabase::set_remove(trn_id_t trn_id, const key_type &key, size_t count) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_remove, key, count);
	return Database::set_remove(key, count);
}

std::optional<size_t> LockedDatabase::set_discard(trn_id_t trn_id, const key_type &key, const set_t &values) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_discard, key, values);
	return Database::set_discard(key, values);
}

std::optional<size_t> LockedDatabase::set_len(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_len, key);
	return Database::set_len(key);
}

std::optional<bool> LockedDatabase::set_contains(trn_id_t trn_id, const key_type &key, const std::string &value) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_contains, key, value);
	return Database::set_contains(key, value);
}

std::optional<bool> LockedDatabase::set_move(trn_id_t trn_id, const key_type &source, const key_type &dest, const std::string &value) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_move, source, dest, value);
	return Database::set_move(source, dest, value);
}

std::optional<set_t> LockedDatabase::set_union(trn_id_t trn_id, const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_union, keys);
	return Database::set_union(keys);
}

std::optional<size_t>
LockedDatabase::set_union_into(trn_id_t trn_id, const key_type &dest, const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_union_into, dest, keys);
	return Database::set_union_into(dest, keys);
}

std::optional<size_t> LockedDatabase::set_union_len(trn_id_t trn_id, const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_union_len, keys);
	return Database::set_union_len(keys);
}

std::optional<set_t> LockedDatabase::set_intersection(trn_id_t trn_id, const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_intersection, keys);
	return Database::set_intersection(keys);
}

std::optional<size_t>
LockedDatabase::set_intersection_into(trn_id_t trn_id, const key_type &dest, const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_intersection_into, dest, keys);
	return Database::set_intersection_into(dest, keys);
}

std::optional<size_t> LockedDatabase::set_intersection_len(trn_id_t trn_id, const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_intersection_len, keys);
	return Database::set_intersection_len(keys);
}

std::optional<set_t>
LockedDatabase::set_difference(trn_id_t trn_id, const key_type &key1, const key_type &key2) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_difference, key1, key2);
	return Database::set_difference(key1, key2);
}

std::optional<size_t> LockedDatabase::set_difference_into(trn_id_t trn_id, const key_type &dest, const key_type &key1, const key_type &key2) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_difference_into, dest, key1, key2);
	return Database::set_difference_into(dest, key1, key2);
}

std::optional<size_t>
LockedDatabase::set_difference_len(trn_id_t trn_id, const key_type &key1, const key_type &key2) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::set_difference_len, key1, key2);
	return Database::set_difference_len(key1, key2);
}


void LockedDatabase::hash_set(trn_id_t trn_id, const key_type &key, hash_t values) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_set, key, values);
	Database::hash_set(key, std::move(values));
}

std::variant<hash_t, HashError> LockedDatabase::hash_all(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_all, key);
	return Database::hash_all(key);
}

std::variant<string_t, HashError>
LockedDatabase::hash_get(trn_id_t trn_id, const key_type &key, const string_t &hash_key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_get, key, hash_key);
	return Database::hash_get(key, hash_key);
}

std::variant<bool, HashError>
LockedDatabase::hash_contains(trn_id_t trn_id, const key_type &key, const string_t &hash_key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_contains, key, hash_key);
	return Database::hash_contains(key, hash_key);
}

std::variant<size_t, HashError> LockedDatabase::hash_len(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_len, key);
	return Database::hash_len(key);
}

std::variant<size_t, HashError>
LockedDatabase::hash_key_len(trn_id_t trn_id, const key_type &key, const string_t &hash_key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_key_len, key, hash_key);
	return Database::hash_key_len(key, hash_key);
}

std::variant<size_t, HashError>
LockedDatabase::hash_remove(trn_id_t trn_id, const key_type &key, const std::vector<string_t> &hash_keys) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_remove, key, hash_keys);
	return Database::hash_remove(key, hash_keys);
}

std::variant<std::vector<string_t>, HashError> LockedDatabase::hash_keys(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_keys, key);
	return Database::hash_keys(key);
}

std::variant<std::vector<string_t>, HashError> LockedDatabase::hash_values(trn_id_t trn_id, const key_type &key) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_values, key);
	return Database::hash_values(key);
}

std::variant<size_t, HashError> LockedDatabase::hash_update(trn_id_t trn_id, const key_type &key, hash_t values) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_update, key, values);
	return Database::hash_update(key, std::move(values));
}

std::variant<std::vector<std::optional<string_t>>, HashError>
LockedDatabase::hash_many_get(trn_id_t trn_id, const key_type &key, const std::vector<string_t> &hash_keys) {
	std::lock_guard lock{m_mutex};
	wal_log(trn_id, db_op_t::hash_many_get, key, hash_keys);
	return Database::hash_many_get(key, hash_keys);
}

} // namespace vanity::db

#pragma clang diagnostic pop
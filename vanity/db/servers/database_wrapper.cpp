//
// Created by kingsli on 9/1/24.
//

#include "database_wrapper.h"

namespace vanity::db {

DatabaseWrapper::DatabaseWrapper(LockedDatabase &database, trn_id_t trn_id)
	: m_database(database), m_trn_id(trn_id) {}

void DatabaseWrapper::reset() {
	m_database.reset(m_trn_id);
}

bool DatabaseWrapper::has(const key_type &key) {
	return m_database.has(m_trn_id, key);
}

bool DatabaseWrapper::del(const key_type &key) {
	return m_database.del(m_trn_id, key);
}

std::optional<db::db_data_type> DatabaseWrapper::get(const key_type &key) {
	return m_database.get(m_trn_id, key);
}

std::optional<int> DatabaseWrapper::type(const key_type &key) {
	return m_database.type(m_trn_id, key);
}

std::vector<db::db_key_type> DatabaseWrapper::keys() {
	return m_database.keys(m_trn_id);
}

bool DatabaseWrapper::copy_to(const key_type &from, const key_type &to) {
	return m_database.copy_to(m_trn_id, from, to);
}

bool DatabaseWrapper::move_to(const key_type &from, const key_type &to) {
	return m_database.move_to(m_trn_id, from, to);
}

bool DatabaseWrapper::copy_to_db(const key_type &from, LockedDatabase &to) {
	return m_database.copy_to_db(m_trn_id, from, to);
}

bool DatabaseWrapper::move_to_db(const key_type &from, LockedDatabase &to) {
	return m_database.move_to_db(m_trn_id, from, to);
}


void DatabaseWrapper::set_expiry(const key_type &key, time_t expiry_time) {
	m_database.set_expiry(m_trn_id, key, expiry_time);
}

std::optional<time_t> DatabaseWrapper::get_expiry(const key_type &key) {
	return m_database.get_expiry(m_trn_id, key);
}

void DatabaseWrapper::clear_expiry(const key_type &key) {
	m_database.clear_expiry(m_trn_id, key);
}

void DatabaseWrapper::clear_all_expiry() {
	m_database.clear_all_expiry(m_trn_id);
}


void DatabaseWrapper::str_set(const key_type &key, std::string value) {
	m_database.str_set(m_trn_id, key, std::move(value));
}

void DatabaseWrapper::int_set(const key_type &key, int_t value) {
	m_database.int_set(m_trn_id, key, value);
}

void DatabaseWrapper::float_set(const key_type &key, float_t value) {
	m_database.float_set(m_trn_id, key, value);
}

std::optional<int_t> DatabaseWrapper::incr_int(const key_type &key, int_t value) {
	return m_database.incr_int(m_trn_id, key, value);
}

std::optional<float_t> DatabaseWrapper::incr_float(const key_type &key, float_t value) {
	return m_database.incr_float(m_trn_id, key, value);
}

std::optional<int_t> DatabaseWrapper::str_len(const key_type &key) {
	return m_database.str_len(m_trn_id, key);
}

std::vector<std::optional<db::db_data_type>> DatabaseWrapper::many_get(const std::vector<key_type> &keys) {
	return m_database.many_get(m_trn_id, keys);
}


std::variant<size_t, ListErrorKind> DatabaseWrapper::list_len(const key_type &key) {
	return m_database.list_len(m_trn_id, key);
}

std::variant<std::string, ListErrorKind> DatabaseWrapper::list_get(const key_type &key, int64_t index) {
	return m_database.list_get(m_trn_id, key, index);
}

std::variant<bool, ListErrorKind>
DatabaseWrapper::list_set(const key_type &key, int64_t index, std::string value) {
	return m_database.list_set(m_trn_id, key, index, std::move(value));
}

std::variant<size_t, ListErrorKind> DatabaseWrapper::list_push_left(const key_type &key, list_t values) {
	return m_database.list_push_left(m_trn_id, key, std::move(values));
}

std::variant<size_t, ListErrorKind> DatabaseWrapper::list_push_right(const key_type &key, list_t values) {
	return m_database.list_push_right(m_trn_id, key, std::move(values));
}

std::variant<list_t, ListErrorKind> DatabaseWrapper::list_pop_left(const key_type &key, int64_t n) {
	return m_database.list_pop_left(m_trn_id, key, n);
}

std::variant<list_t, ListErrorKind> DatabaseWrapper::list_pop_right(const key_type &key, int64_t n) {
	return m_database.list_pop_right(m_trn_id, key, n);
}

std::variant<list_t, ListErrorKind>
DatabaseWrapper::list_range(const key_type &key, int64_t start, int64_t end) {
	return m_database.list_range(m_trn_id, key, start, end);
}

std::variant<size_t, ListErrorKind>
DatabaseWrapper::list_trim(const key_type &key, int64_t start, int64_t end) {
	return m_database.list_trim(m_trn_id, key, start, end);

}

std::variant<size_t, ListErrorKind>
DatabaseWrapper::list_remove(const key_type &key, const std::string &element, int64_t count) {
	return m_database.list_remove(m_trn_id, key, element, count);
}


std::optional<size_t> DatabaseWrapper::set_add(const key_type &key, set_t values) {
	return m_database.set_add(m_trn_id, key, std::move(values));
}

std::optional<set_t> DatabaseWrapper::set_all(const key_type &key) {
	return m_database.set_all(m_trn_id, key);
}

std::optional<set_t> DatabaseWrapper::set_remove(const key_type &key, size_t count) {
	return m_database.set_remove(m_trn_id, key, count);
}

std::optional<size_t> DatabaseWrapper::set_discard(const key_type &key, const set_t &values) {
	return m_database.set_discard(m_trn_id, key, values);
}

std::optional<size_t> DatabaseWrapper::set_len(const key_type &key) {
	return m_database.set_len(m_trn_id, key);
}

std::optional<bool> DatabaseWrapper::set_contains(const key_type &key, const std::string &value) {
	return m_database.set_contains(m_trn_id, key, value);
}

std::optional<bool> DatabaseWrapper::set_move(const key_type &source, const key_type &dest, const std::string &value) {
	return m_database.set_move(m_trn_id, source, dest, value);
}

std::optional<set_t> DatabaseWrapper::set_union(const std::vector<key_type> &keys) {
	return m_database.set_union(m_trn_id, keys);
}

std::optional<size_t>
DatabaseWrapper::set_union_into(const key_type &dest, const std::vector<key_type> &keys) {
	return m_database.set_union_into(m_trn_id, dest, keys);
}

std::optional<size_t> DatabaseWrapper::set_union_len(const std::vector<key_type> &keys) {
	return m_database.set_union_len(m_trn_id, keys);
}

std::optional<set_t> DatabaseWrapper::set_intersection(const std::vector<key_type> &keys) {
	return m_database.set_intersection(m_trn_id, keys);
}

std::optional<size_t>
DatabaseWrapper::set_intersection_into(const key_type &dest, const std::vector<key_type> &keys) {
	return m_database.set_intersection_into(m_trn_id, dest, keys);
}

std::optional<size_t> DatabaseWrapper::set_intersection_len(const std::vector<key_type> &keys) {
	return m_database.set_intersection_len(m_trn_id, keys);
}

std::optional<set_t>
DatabaseWrapper::set_difference(const key_type &key1, const key_type &key2) {
	return m_database.set_difference(m_trn_id, key1, key2);
}

std::optional<size_t> DatabaseWrapper::set_difference_into(const key_type &dest, const key_type &key1, const key_type &key2) {
	return m_database.set_difference_into(m_trn_id, dest, key1, key2);
}

std::optional<size_t>
DatabaseWrapper::set_difference_len(const key_type &key1, const key_type &key2) {
	return m_database.set_difference_len(m_trn_id, key1, key2);
}


void DatabaseWrapper::hash_set(const key_type &key, hash_t values) {
	m_database.hash_set(m_trn_id, key, std::move(values));
}

std::variant<hash_t, HashError> DatabaseWrapper::hash_all(const key_type &key) {
	return m_database.hash_all(m_trn_id, key);
}

std::variant<string_t, HashError>
DatabaseWrapper::hash_get(const key_type &key, const string_t &hash_key) {
	return m_database.hash_get(m_trn_id, key, hash_key);
}

std::variant<bool, HashError>
DatabaseWrapper::hash_contains(const key_type &key, const string_t &hash_key) {
	return m_database.hash_contains(m_trn_id, key, hash_key);
}

std::variant<size_t, HashError> DatabaseWrapper::hash_len(const key_type &key) {
	return m_database.hash_len(m_trn_id, key);
}

std::variant<size_t, HashError>
DatabaseWrapper::hash_key_len(const key_type &key, const string_t &hash_key) {
	return m_database.hash_key_len(m_trn_id, key, hash_key);
}

std::variant<size_t, HashError>
DatabaseWrapper::hash_remove(const key_type &key, const std::vector<string_t> &hash_keys) {
	return m_database.hash_remove(m_trn_id, key, hash_keys);
}

std::variant<std::vector<string_t>, HashError> DatabaseWrapper::hash_keys(const key_type &key) {
	return m_database.hash_keys(m_trn_id, key);
}

std::variant<std::vector<string_t>, HashError> DatabaseWrapper::hash_values(const key_type &key) {
	return m_database.hash_values(m_trn_id, key);
}

std::variant<size_t, HashError> DatabaseWrapper::hash_update(const key_type &key, hash_t values) {
	return m_database.hash_update(m_trn_id, key, std::move(values));
}

std::variant<std::vector<std::optional<string_t>>, HashError>
DatabaseWrapper::hash_many_get(const key_type &key, const std::vector<string_t> &hash_keys) {
	return m_database.hash_many_get(m_trn_id, key, hash_keys);
}

} // namespace vanity::db

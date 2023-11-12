//
// Created by kingsli on 11/11/23.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#include "locked_database.h"

namespace vanity::db {

LockedDatabase::LockedDatabase() = default;

LockedDatabase::LockedDatabase(Database &&other) noexcept: Database{std::move(other)} {}

LockedDatabase &LockedDatabase::operator=(Database &&other) noexcept {
	Database::operator=(std::move(other));
	return *this;
}

LockedDatabase LockedDatabase::from(std::ifstream &in) {
	return LockedDatabase{Database::from(in)};
}


void LockedDatabase::persist(std::ofstream &out) {
	std::lock_guard lock{m_mutex};
	Database::persist(out);
}

void LockedDatabase::reset() {
	std::lock_guard lock{m_mutex};
	Database::reset();
}

bool LockedDatabase::has(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::has(key);
}

bool LockedDatabase::del(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::del(key);
}

std::optional<Database::data_type> LockedDatabase::get(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::get(key);
}

std::optional<int> LockedDatabase::type(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::type(key);
}


void LockedDatabase::set(const key_type &key, const data_type &value) {
	std::lock_guard lock{m_mutex};
	Database::set(key, value);
}

std::optional<int_t> LockedDatabase::incr_int(const BaseDatabase::key_type &key, int_t value) {
	std::lock_guard lock{m_mutex};
	return Database::incr_int(key, value);
}

std::optional<float_t> LockedDatabase::incr_float(const BaseDatabase::key_type &key, float_t value) {
	std::lock_guard lock{m_mutex};
	return Database::incr_float(key, value);
}

std::optional<int_t> LockedDatabase::str_len(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::str_len(key);
}

std::vector<std::optional<Database::data_type>> LockedDatabase::many_get(const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	return Database::many_get(keys);
}

void LockedDatabase::many_set(std::vector<std::pair<db_key_type, db_data_type>> keys) {
	std::lock_guard lock{m_mutex};
	Database::many_set(std::move(keys));
}


std::variant<size_t, ListErrorKind> LockedDatabase::list_len(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::list_len(key);
}

std::variant<std::string, ListErrorKind> LockedDatabase::list_get(const key_type &key, int64_t index) {
	std::lock_guard lock{m_mutex};
	return Database::list_get(key, index);
}

std::variant<bool, ListErrorKind>
LockedDatabase::list_set(const BaseDatabase::key_type &key, int64_t index, const std::string &value) {
	std::lock_guard lock{m_mutex};
	return Database::list_set(key, index, value);
}

std::variant<size_t, ListErrorKind> LockedDatabase::list_push_left(const BaseDatabase::key_type &key, list_t values) {
	std::lock_guard lock{m_mutex};
	return Database::list_push_left(key, std::move(values));
}

std::variant<size_t, ListErrorKind> LockedDatabase::list_push_right(const BaseDatabase::key_type &key, list_t values) {
	std::lock_guard lock{m_mutex};
	return Database::list_push_right(key, std::move(values));
}

std::variant<list_t, ListErrorKind> LockedDatabase::list_pop_left(const key_type &key, int64_t n) {
	std::lock_guard lock{m_mutex};
	return Database::list_pop_left(key, n);
}

std::variant<list_t, ListErrorKind> LockedDatabase::list_pop_right(const key_type &key, int64_t n) {
	std::lock_guard lock{m_mutex};
	return Database::list_pop_right(key, n);
}

std::variant<list_t, ListErrorKind>
LockedDatabase::list_range(const BaseDatabase::key_type &key, int64_t start, int64_t end) {
	std::lock_guard lock{m_mutex};
	return Database::list_range(key, start, end);
}

std::variant<size_t, ListErrorKind>
LockedDatabase::list_trim(const BaseDatabase::key_type &key, int64_t start, int64_t end) {
	std::lock_guard lock{m_mutex};
	return Database::list_trim(key, start, end);

}

std::variant<size_t, ListErrorKind>
LockedDatabase::list_remove(const BaseDatabase::key_type &key, const std::string &element, int64_t count) {
	std::lock_guard lock{m_mutex};
	return Database::list_remove(key, element, count);
}


std::optional<size_t> LockedDatabase::set_add(const BaseDatabase::key_type &key, set_t values) {
	std::lock_guard lock{m_mutex};
	return Database::set_add(key, std::move(values));
}

std::optional<set_t> LockedDatabase::set_all(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::set_all(key);
}

std::optional<set_t> LockedDatabase::set_remove(const BaseDatabase::key_type &key, size_t count) {
	std::lock_guard lock{m_mutex};
	return Database::set_remove(key, count);
}

std::optional<size_t> LockedDatabase::set_discard(const BaseDatabase::key_type &key, const set_t &values) {
	std::lock_guard lock{m_mutex};
	return Database::set_discard(key, values);
}

std::optional<size_t> LockedDatabase::set_len(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::set_len(key);
}

std::optional<bool> LockedDatabase::set_contains(const BaseDatabase::key_type &key, const std::string &value) {
	std::lock_guard lock{m_mutex};
	return Database::set_contains(key, value);
}

std::optional<bool> LockedDatabase::set_move(
	const BaseDatabase::key_type &source,
	const BaseDatabase::key_type &dest,
	const std::string &value)
{
	std::lock_guard lock{m_mutex};
	return Database::set_move(source, dest, value);
}

std::optional<set_t> LockedDatabase::set_union(const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	return Database::set_union(keys);
}

std::optional<size_t>
LockedDatabase::set_union_into(const BaseDatabase::key_type &dest, const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	return Database::set_union_into(dest, keys);
}

std::optional<size_t> LockedDatabase::set_union_len(const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	return Database::set_union_len(keys);
}

std::optional<set_t> LockedDatabase::set_intersection(const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	return Database::set_intersection(keys);
}

std::optional<size_t>
LockedDatabase::set_intersection_into(const BaseDatabase::key_type &dest, const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	return Database::set_intersection_into(dest, keys);
}

std::optional<size_t> LockedDatabase::set_intersection_len(const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	return Database::set_intersection_len(keys);
}

std::optional<set_t>
LockedDatabase::set_difference(const BaseDatabase::key_type &key1, const BaseDatabase::key_type &key2) {
	std::lock_guard lock{m_mutex};
	return Database::set_difference(key1, key2);
}

std::optional<size_t> LockedDatabase::set_difference_into(
	const BaseDatabase::key_type &dest,
	const BaseDatabase::key_type &key1,
	const BaseDatabase::key_type &key2)
{
	std::lock_guard lock{m_mutex};
	return Database::set_difference_into(dest, key1, key2);
}

std::optional<size_t>
LockedDatabase::set_difference_len(const BaseDatabase::key_type &key1, const BaseDatabase::key_type &key2) {
	std::lock_guard lock{m_mutex};
	return Database::set_difference_len(key1, key2);
}


void LockedDatabase::hash_set(const BaseDatabase::key_type &key, hash_t values) {
	std::lock_guard lock{m_mutex};
	Database::hash_set(key, std::move(values));
}

std::variant<hash_t, HashError> LockedDatabase::hash_all(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_all(key);
}

std::variant<string_t, HashError>
LockedDatabase::hash_get(const BaseDatabase::key_type &key, const string_t &hash_key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_get(key, hash_key);
}

std::variant<bool, HashError>
LockedDatabase::hash_contains(const BaseDatabase::key_type &key, const string_t &hash_key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_contains(key, hash_key);
}

std::variant<size_t, HashError> LockedDatabase::hash_len(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_len(key);
}

std::variant<size_t, HashError>
LockedDatabase::hash_key_len(const BaseDatabase::key_type &key, const string_t &hash_key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_key_len(key, hash_key);
}

std::variant<size_t, HashError>
LockedDatabase::hash_remove(const BaseDatabase::key_type &key, const std::vector<string_t> &hash_keys) {
	std::lock_guard lock{m_mutex};
	return Database::hash_remove(key, hash_keys);
}

std::variant<std::vector<string_t>, HashError> LockedDatabase::hash_keys(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_keys(key);
}

std::variant<std::vector<string_t>, HashError> LockedDatabase::hash_values(const BaseDatabase::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_values(key);
}

std::variant<size_t, HashError> LockedDatabase::hash_update(const BaseDatabase::key_type &key, hash_t values) {
	std::lock_guard lock{m_mutex};
	return Database::hash_update(key, std::move(values));
}

std::variant<std::vector<std::optional<string_t>>, HashError>
LockedDatabase::hash_many_get(const BaseDatabase::key_type &key, const std::vector<string_t> &hash_keys) {
	std::lock_guard lock{m_mutex};
	return Database::hash_many_get(key, hash_keys);
}

} // namespace vanity::db

#pragma clang diagnostic pop
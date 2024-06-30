//
// Created by kingsli on 11/11/23.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#include "locked_database.h"

namespace vanity::db {

LockedDatabase::LockedDatabase(m_index_type index)
	:Database{index} {}

LockedDatabase::LockedDatabase(Database &&other) noexcept
	:Database{std::move(other)} {}

LockedDatabase &LockedDatabase::operator=(Database &&other) noexcept {
	Database::operator=(std::move(other));
	return *this;
}

LockedDatabase LockedDatabase::from(std::ifstream &in) {
	return LockedDatabase{Database::from(in)};
}


std::lock_guard<LockedDatabase::lock_type> LockedDatabase::lock() {
	return std::lock_guard{m_mutex};
}

auto LockedDatabase::mutex() -> lock_type& {
	return m_mutex;
}


void LockedDatabase::persist(std::ofstream &out) {
	std::lock_guard lock{m_mutex};
	Database::persist(out);
}

void LockedDatabase::reset() {
	std::lock_guard lock{m_mutex};
	Database::reset();
}

bool LockedDatabase::has(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::has(key);
}

bool LockedDatabase::del(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::del(key);
}

std::optional<Database::data_type> LockedDatabase::get(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::get(key);
}

std::optional<int> LockedDatabase::type(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::type(key);
}

std::vector<Database::key_type> LockedDatabase::keys() {
	std::lock_guard lock{m_mutex};
	return Database::keys();
}

bool LockedDatabase::copy_to(const key_type &from, const key_type &to) {
	std::lock_guard lock{m_mutex};
	return Database::copy_to(from, to);
}

bool LockedDatabase::move_to(const key_type &from, const key_type &to) {
	std::lock_guard lock{m_mutex};
	return Database::move_to(from, to);
}

bool LockedDatabase::copy_to_db(const key_type &from, LockedDatabase &to) {
	std::scoped_lock lock{m_mutex, to.m_mutex};
	return Database::copy_to_db(from, to);
}

bool LockedDatabase::move_to_db(const key_type &from, LockedDatabase &to) {
	std::scoped_lock lock{m_mutex, to.m_mutex};
	return Database::move_to_db(from, to);
}


bool LockedDatabase::is_expired(const vanity::db::BaseMap::key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::is_expired(key);
}

void LockedDatabase::erase_if_expired(const vanity::db::BaseMap::key_type &key) {
	std::lock_guard lock{m_mutex};
	Database::erase_if_expired(key);
}

void LockedDatabase::set_expiry(const key_type &key, time_t expiry_time) {
	std::lock_guard lock{m_mutex};
	Database::set_expiry(key, expiry_time);
}

std::optional<time_t> LockedDatabase::get_expiry(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::get_expiry(key);
}

void LockedDatabase::clear_expiry(const key_type &key) {
	std::lock_guard lock{m_mutex};
	Database::clear_expiry(key);
}

void LockedDatabase::clear_all_expiry() {
	std::lock_guard lock{m_mutex};
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

void LockedDatabase::on_expire(callback_t callback) {
	std::lock_guard lock{m_mutex};
	Database::on_expire(std::move(callback));
}

void LockedDatabase::disable_on_expire() {
	std::lock_guard lock{m_mutex};
	Database::disable_on_expire();
}

void LockedDatabase::expiry_enabled(bool enable) {
	std::lock_guard lock{m_mutex};
	Database::expiry_enabled(enable);
}

void LockedDatabase::force_expire(const key_type &key) {
	std::lock_guard lock{m_mutex};
	Database::force_expire(key);
}


void LockedDatabase::str_set(const key_type &key, const std::string &value) {
	std::lock_guard lock{m_mutex};
	Database::str_set(key, value);
}

void LockedDatabase::int_set(const key_type &key, int_t value) {
	std::lock_guard lock{m_mutex};
	Database::int_set(key, value);
}

void LockedDatabase::float_set(const key_type &key, float_t value) {
	std::lock_guard lock{m_mutex};
	Database::float_set(key, value);
}

std::optional<int_t> LockedDatabase::incr_int(const key_type &key, int_t value) {
	std::lock_guard lock{m_mutex};
	return Database::incr_int(key, value);
}

std::optional<float_t> LockedDatabase::incr_float(const key_type &key, float_t value) {
	std::lock_guard lock{m_mutex};
	return Database::incr_float(key, value);
}

std::optional<int_t> LockedDatabase::str_len(const key_type &key) {
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
LockedDatabase::list_set(const key_type &key, int64_t index, const std::string &value) {
	std::lock_guard lock{m_mutex};
	return Database::list_set(key, index, value);
}

std::variant<size_t, ListErrorKind> LockedDatabase::list_push_left(const key_type &key, list_t values) {
	std::lock_guard lock{m_mutex};
	return Database::list_push_left(key, std::move(values));
}

std::variant<size_t, ListErrorKind> LockedDatabase::list_push_right(const key_type &key, list_t values) {
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
LockedDatabase::list_range(const key_type &key, int64_t start, int64_t end) {
	std::lock_guard lock{m_mutex};
	return Database::list_range(key, start, end);
}

std::variant<size_t, ListErrorKind>
LockedDatabase::list_trim(const key_type &key, int64_t start, int64_t end) {
	std::lock_guard lock{m_mutex};
	return Database::list_trim(key, start, end);

}

std::variant<size_t, ListErrorKind>
LockedDatabase::list_remove(const key_type &key, const std::string &element, int64_t count) {
	std::lock_guard lock{m_mutex};
	return Database::list_remove(key, element, count);
}


std::optional<size_t> LockedDatabase::set_add(const key_type &key, set_t values) {
	std::lock_guard lock{m_mutex};
	return Database::set_add(key, std::move(values));
}

std::optional<set_t> LockedDatabase::set_all(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::set_all(key);
}

std::optional<set_t> LockedDatabase::set_remove(const key_type &key, size_t count) {
	std::lock_guard lock{m_mutex};
	return Database::set_remove(key, count);
}

std::optional<size_t> LockedDatabase::set_discard(const key_type &key, const set_t &values) {
	std::lock_guard lock{m_mutex};
	return Database::set_discard(key, values);
}

std::optional<size_t> LockedDatabase::set_len(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::set_len(key);
}

std::optional<bool> LockedDatabase::set_contains(const key_type &key, const std::string &value) {
	std::lock_guard lock{m_mutex};
	return Database::set_contains(key, value);
}

std::optional<bool> LockedDatabase::set_move(
	const key_type &source,
	const key_type &dest,
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
LockedDatabase::set_union_into(const key_type &dest, const std::vector<key_type> &keys) {
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
LockedDatabase::set_intersection_into(const key_type &dest, const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	return Database::set_intersection_into(dest, keys);
}

std::optional<size_t> LockedDatabase::set_intersection_len(const std::vector<key_type> &keys) {
	std::lock_guard lock{m_mutex};
	return Database::set_intersection_len(keys);
}

std::optional<set_t>
LockedDatabase::set_difference(const key_type &key1, const key_type &key2) {
	std::lock_guard lock{m_mutex};
	return Database::set_difference(key1, key2);
}

std::optional<size_t> LockedDatabase::set_difference_into(
	const key_type &dest,
	const key_type &key1,
	const key_type &key2)
{
	std::lock_guard lock{m_mutex};
	return Database::set_difference_into(dest, key1, key2);
}

std::optional<size_t>
LockedDatabase::set_difference_len(const key_type &key1, const key_type &key2) {
	std::lock_guard lock{m_mutex};
	return Database::set_difference_len(key1, key2);
}


void LockedDatabase::hash_set(const key_type &key, hash_t values) {
	std::lock_guard lock{m_mutex};
	Database::hash_set(key, std::move(values));
}

std::variant<hash_t, HashError> LockedDatabase::hash_all(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_all(key);
}

std::variant<string_t, HashError>
LockedDatabase::hash_get(const key_type &key, const string_t &hash_key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_get(key, hash_key);
}

std::variant<bool, HashError>
LockedDatabase::hash_contains(const key_type &key, const string_t &hash_key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_contains(key, hash_key);
}

std::variant<size_t, HashError> LockedDatabase::hash_len(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_len(key);
}

std::variant<size_t, HashError>
LockedDatabase::hash_key_len(const key_type &key, const string_t &hash_key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_key_len(key, hash_key);
}

std::variant<size_t, HashError>
LockedDatabase::hash_remove(const key_type &key, const std::vector<string_t> &hash_keys) {
	std::lock_guard lock{m_mutex};
	return Database::hash_remove(key, hash_keys);
}

std::variant<std::vector<string_t>, HashError> LockedDatabase::hash_keys(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_keys(key);
}

std::variant<std::vector<string_t>, HashError> LockedDatabase::hash_values(const key_type &key) {
	std::lock_guard lock{m_mutex};
	return Database::hash_values(key);
}

std::variant<size_t, HashError> LockedDatabase::hash_update(const key_type &key, hash_t values) {
	std::lock_guard lock{m_mutex};
	return Database::hash_update(key, std::move(values));
}

std::variant<std::vector<std::optional<string_t>>, HashError>
LockedDatabase::hash_many_get(const key_type &key, const std::vector<string_t> &hash_keys) {
	std::lock_guard lock{m_mutex};
	return Database::hash_many_get(key, hash_keys);
}

} // namespace vanity::db

#pragma clang diagnostic pop
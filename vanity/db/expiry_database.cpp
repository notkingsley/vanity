//
// Created by kingsli on 11/22/23.
//

#include "expiry_database.h"

namespace vanity::db {

ExpiryDatabase::ExpiryDatabase() = default;

bool ExpiryDatabase::is_expired(const key_type &key) {
	if (not m_expiry_times.contains(key))
		return false;
	return std::chrono::system_clock::now() > m_expiry_times[key];
}

void ExpiryDatabase::erase_if_expired(const key_type &key) {
	if (is_expired(key)){
		m_data.erase(key);
		m_expiry_times.erase(key);
	}
}

void ExpiryDatabase::clear_expiry(const key_type &key) {
	m_expiry_times.erase(key);
}

void ExpiryDatabase::set_expiry(const key_type &key, time_t expiry_time) {
	m_expiry_times[key] = expiry_time;
}

std::optional<time_t> ExpiryDatabase::get_expiry(const BaseMap::key_type &key) {
	if (not m_expiry_times.contains(key))
		return std::nullopt;
	return m_expiry_times[key];
}

void ExpiryDatabase::clear_all_expiry() {
	m_expiry_times.clear();
}

} // namespace vanity::db

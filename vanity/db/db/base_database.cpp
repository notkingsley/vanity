//
// Created by kingsli on 10/29/23.
//

#include "base_database.h"

namespace vanity::db {

bool BaseDatabase::has(const key_type& key) {
	erase_if_expired(key);
	return m_data.contains(key);
}

auto BaseDatabase::get(const key_type& key) -> std::optional<const data_type> {
	erase_if_expired(key);
	if (m_data.contains(key))
		return m_data.at(key);
	return std::nullopt;
}

void BaseDatabase::reset() {
	m_data.clear();
	clear_all_expiry();
}

bool BaseDatabase::del(const key_type& key) {
	clear_expiry(key);
	return m_data.erase(key);
}

std::optional<int> BaseDatabase::type(const key_type &key) {
	erase_if_expired(key);
	if (m_data.contains(key))
		return m_data.at(key).index();
	return std::nullopt;
}

} // namespace vanity::db
//
// Created by kingsli on 10/29/23.
//

#include "base_database.h"

namespace vanity::db {

bool BaseDatabase::has(const key_type& key) const {
	return m_data.contains(key);
}

void BaseDatabase::reset() {
	m_data.clear();
}

bool BaseDatabase::del(const key_type& key) {
	return m_data.erase(key);
}

std::optional<int> BaseDatabase::type(const key_type &key) {
	if (m_data.contains(key))
		return m_data.at(key).index();
	return std::nullopt;
}

} // namespace vanity::db
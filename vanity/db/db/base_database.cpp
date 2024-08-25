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

std::vector<BaseDatabase::key_type> BaseDatabase::keys() {
	deep_purge();
	std::vector<key_type> keys;
	keys.reserve(m_data.size());
	for (auto& [key, _] : m_data)
		keys.push_back(key);

	return keys;
}

bool BaseDatabase::copy_to(const key_type &from, const key_type &to) {
	erase_if_expired(from);
	if (not m_data.contains(from))
		return false;

	if (from == to)
		return true;

	m_data[to] = m_data.at(from);
	clear_expiry(to);
	return true;
}

bool BaseDatabase::move_to(const key_type &from, const key_type &to) {
	erase_if_expired(from);
	if (not m_data.contains(from))
		return false;

	if (from == to)
		return true;

	m_data[to] = std::move(m_data.at(from));
	if (m_expiry_times.contains(from))
		m_expiry_times[to] = m_expiry_times.at(from);

	del(from);
	return true;
}

bool BaseDatabase::copy_to_db(const key_type &from, BaseDatabase &to) {
	erase_if_expired(from);
	if (not m_data.contains(from))
		return false;

	if (this == &to)
		return true;

	to.m_data[from] = m_data.at(from);
	to.clear_expiry(from);
	return true;
}

bool BaseDatabase::move_to_db(const key_type &from, BaseDatabase &to) {
	erase_if_expired(from);
	if (not m_data.contains(from))
		return false;

	if (this == &to)
		return true;

	to.m_data[from] = std::move(m_data.at(from));
	if (m_expiry_times.contains(from))
		to.m_expiry_times[from] = m_expiry_times.at(from);

	del(from);
	return true;
}

} // namespace vanity::db
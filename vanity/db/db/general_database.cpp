//
// Created by kingsli on 9/3/24.
//

#include "general_database.h"

namespace vanity::db {

GeneralDatabase::GeneralDatabase() = default;

GeneralDatabase::GeneralDatabase(GeneralDatabase &&other) noexcept
	: BaseDatabase(std::move(other)) {}

GeneralDatabase &GeneralDatabase::operator=(GeneralDatabase &&other) noexcept {
	BaseDatabase::operator=(std::move(other));
	return *this;
}

bool GeneralDatabase::has(const key_type& key) {
	erase_if_expired(key);
	return m_data.contains(key);
}

auto GeneralDatabase::get(const key_type& key) -> std::optional<const data_type> {
	return expiry_aware_get(key);
}

void GeneralDatabase::reset() {
	m_data.clear();
	clear_all_expiry();
}

bool GeneralDatabase::del(const key_type& key) {
	return expiry_aware_del(key);
}

std::optional<int> GeneralDatabase::type(const key_type &key) {
	erase_if_expired(key);
	if (m_data.contains(key))
		return m_data.at(key).index();
	return std::nullopt;
}

std::vector<BaseDatabase::key_type> GeneralDatabase::keys() {
	deep_purge();
	std::vector<key_type> keys;
	keys.reserve(m_data.size());
	for (auto& [key, _] : m_data)
		keys.push_back(key);

	return keys;
}

bool GeneralDatabase::copy_to(const key_type &from, const key_type &to) {
	erase_if_expired(from);
	if (not m_data.contains(from))
		return false;

	if (from == to)
		return true;

	m_data[to] = m_data.at(from);
	clear_expiry(to);
	return true;
}

bool GeneralDatabase::move_to(const key_type &from, const key_type &to) {
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

bool GeneralDatabase::copy_to_db(const key_type &from, GeneralDatabase &to) {
	erase_if_expired(from);
	if (not m_data.contains(from))
		return false;

	if (this == &to)
		return true;

	to.m_data[from] = m_data.at(from);
	to.clear_expiry(from);
	return true;
}

bool GeneralDatabase::move_to_db(const key_type &from, GeneralDatabase &to) {
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

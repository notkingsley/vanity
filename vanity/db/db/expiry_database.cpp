//
// Created by kingsli on 11/22/23.
//

#include <random>
#include <ranges>
#include <utility>

#include "expiry_database.h"


namespace vanity::db {

ExpiryDatabase::ExpiryDatabase() = default;

void ExpiryDatabase::expire(const key_type& key) {
	pre_expire(key);
	_do_expire(key);
}

void ExpiryDatabase::_do_expire(const key_type &key) {
	m_data.erase(key);
	clear_expiry(key);
}

bool ExpiryDatabase::is_expired(const key_type &key) {
	if (not m_expiry_enabled)
		return false;

	if (not m_expiry_times.contains(key))
		return false;

	return std::chrono::system_clock::now() > m_expiry_times[key];
}

bool ExpiryDatabase::erase_if_expired(const key_type &key) {
	if (not is_expired(key))
		return false;

	expire(key);
	return true;
}

void ExpiryDatabase::clear_expiry(const key_type &key) {
	m_expiry_times.erase(key);
}

void ExpiryDatabase::set_expiry(const key_type &key, time_t expiry_time) {
	if (m_data.contains(key))
		m_expiry_times[key] = expiry_time;
}

std::optional<time_t> ExpiryDatabase::get_expiry(const key_type &key) {
	erase_if_expired(key);
	if (not m_expiry_times.contains(key))
		return std::nullopt;

	return m_expiry_times[key];
}

void ExpiryDatabase::clear_all_expiry() {
	m_expiry_times.clear();
}

void ExpiryDatabase::shallow_purge() {
	static std::random_device rd;
	static std::mt19937 gen(rd());

	while (not m_expiry_times.empty())
	{
		auto sample_size = std::min<size_t>(M_MAX_SAMPLE_SIZE, m_expiry_times.size());
		auto all_keys = std::views::keys(m_expiry_times);
		std::vector<key_type> sampled_keys;
		sampled_keys.reserve(sample_size);

		sample(
			all_keys.begin(),
			all_keys.end(),
			back_inserter(sampled_keys),
			sample_size,
			gen
		);
		size_t expired_count = 0;
		for (auto& key: sampled_keys)
			if (erase_if_expired(key))
				expired_count += 1;

		if (expired_count / sample_size < M_MIN_EXPIRED_PERCENTAGE)
			break;
	}
}

void ExpiryDatabase::deep_purge() {
	std::vector<key_type> expired_keys;
	for (auto& key: std::views::keys(m_expiry_times))
		if (is_expired(key))
			expired_keys.push_back(key);

	for (auto& key: expired_keys)
		expire(key);
}

void ExpiryDatabase::expiry_enabled(bool enable) {
	m_expiry_enabled = enable;
}

void ExpiryDatabase::force_expire(const key_type &key) {
	_do_expire(key);
}

} // namespace vanity::db

//
// Created by kingsli on 11/22/23.
//

#include <random>
#include <ranges>

#include "expiry_database.h"


namespace vanity::db {

ExpiryDatabase::ExpiryDatabase() = default;

void ExpiryDatabase::expire(const key_type& key) {
	m_data.erase(key);
	m_expiry_times.erase(key);
}

bool ExpiryDatabase::is_expired(const key_type &key) {
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

std::optional<time_t> ExpiryDatabase::get_expiry(const BaseMap::key_type &key) {
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

	while (true) {
		std::uniform_int_distribution<size_t> dis(0, m_expiry_times.size() - 1);
		size_t sample_size = std::min<size_t>(M_MAX_SAMPLE_SIZE, m_expiry_times.size());
		auto all_keys = std::views::keys(m_expiry_times);

		std::vector<key_type> sampled_keys;
		sampled_keys.reserve(sample_size);

		sample(all_keys.begin(), all_keys.end(), back_inserter(sampled_keys), sample_size, gen);
		size_t expired_count = std::count_if(
			sampled_keys.begin(),
			sampled_keys.end(),
			[this](const auto& key) {
				return erase_if_expired(key);
			}
		);

		if (expired_count / sample_size < M_MIN_EXPIRED_PERCENTAGE)
			break;

		if (m_expiry_times.empty())
			break;
	}
}

void ExpiryDatabase::deep_purge() {
	for (auto& [key, _] : m_expiry_times)
		erase_if_expired(key);
}

} // namespace vanity::db

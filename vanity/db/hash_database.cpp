//
// Created by kingsli on 11/7/23.
//

#include "hash_database.h"

namespace vanity::db {

HashDatabase::HashDatabase() = default;

HashDatabase::HashDatabase(HashDatabase &&other) noexcept
	: BaseDatabase(std::move(other)) {}

HashDatabase &HashDatabase::operator=(HashDatabase &&other) noexcept {
	BaseDatabase::operator=(std::move(other));
	return *this;
}

void HashDatabase::hash_set(const key_type &key, hash_t values) {
	m_data[key] = std::move(values);
	clear_expiry(key);
}

std::variant<hash_t, HashError> HashDatabase::hash_all(const key_type &key) {
	erase_if_expired(key);
	if (not m_data.contains(key))
		return hash_t{};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<hash_t>(value))
		return HashError::NotHash;

	return std::get<hash_t>(value);
}

std::variant<string_t, HashError> HashDatabase::hash_get(const key_type &key, const string_t &hash_key) {
	erase_if_expired(key);
	if (not m_data.contains(key))
		return HashError::BadKey;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<hash_t>(value))
		return HashError::NotHash;

	auto& hash = std::get<hash_t>(value);
	if (not hash.contains(hash_key))
		return HashError::BadKey;

	return hash.at(hash_key);
}

std::variant<bool, HashError> HashDatabase::hash_contains(const key_type &key, const string_t &hash_key) {
	erase_if_expired(key);
	if (not m_data.contains(key))
		return false;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<hash_t>(value))
		return HashError::NotHash;

	auto& hash = std::get<hash_t>(value);
	return hash.contains(hash_key);
}

std::variant<size_t, HashError> HashDatabase::hash_len(const key_type &key) {
	erase_if_expired(key);
	if (not m_data.contains(key))
		return 0ull;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<hash_t>(value))
		return HashError::NotHash;

	auto& hash = std::get<hash_t>(value);
	return hash.size();
}

std::variant<size_t, HashError> HashDatabase::hash_key_len(const key_type &key, const string_t &hash_key) {
	erase_if_expired(key);
	if (not m_data.contains(key))
		return HashError::BadKey;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<hash_t>(value))
		return HashError::NotHash;

	auto& hash = std::get<hash_t>(value);
	if (not hash.contains(hash_key))
		return HashError::BadKey;

	return hash.at(hash_key).size();
}

std::variant<size_t, HashError> HashDatabase::hash_remove(const key_type &key, const std::vector<string_t> &hash_keys) {
	erase_if_expired(key);
	if (not m_data.contains(key))
		return 0ull;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<hash_t>(value))
		return HashError::NotHash;

	auto& hash = std::get<hash_t>(value);
	size_t size = hash.size();
	for (const auto& hash_key : hash_keys)
		hash.erase(hash_key);

	if (hash.empty()){
		m_data.erase(key);
		clear_expiry(key);
	}
	return size - hash.size();
}

std::variant<std::vector<string_t>, HashError> HashDatabase::hash_keys(const key_type &key) {
	erase_if_expired(key);
	if (not m_data.contains(key))
		return std::vector<string_t>{};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<hash_t>(value))
		return HashError::NotHash;

	auto& hash = std::get<hash_t>(value);
	std::vector<string_t> keys;
	keys.reserve(hash.size());
	for (const auto& [k, _] : hash)
		keys.push_back(k);

	return keys;
}

std::variant<std::vector<string_t>, HashError> HashDatabase::hash_values(const key_type &key) {
	erase_if_expired(key);
	if (not m_data.contains(key))
		return std::vector<string_t>{};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<hash_t>(value))
		return HashError::NotHash;

	auto& hash = std::get<hash_t>(value);
	std::vector<string_t> values;
	values.reserve(hash.size());
	for (const auto& [_, v] : hash)
		values.push_back(v);

	return values;
}

std::variant<size_t, HashError> HashDatabase::hash_update(const key_type &key, hash_t values) {
	erase_if_expired(key);
	if (not m_data.contains(key))
		m_data[key] = {};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<hash_t>(value))
		return HashError::NotHash;

	auto& hash = std::get<hash_t>(value);
	size_t size = hash.size();
	for (auto it = values.begin(); it != values.end(); ++it)
		if (hash.contains(it->first))
			hash[it->first] = it->second;
		else
			hash.insert(values.extract(it));

	return hash.size() - size;
}

std::variant<std::vector<std::optional<string_t>>, HashError>
HashDatabase::hash_many_get(const key_type &key, const std::vector<string_t> &hash_keys) {
	erase_if_expired(key);
	if (not m_data.contains(key))
		return std::vector<std::optional<string_t>>{hash_keys.size(), std::nullopt};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<hash_t>(value))
		return HashError::NotHash;

	auto& hash = std::get<hash_t>(value);
	std::vector<std::optional<string_t>> values;
	values.reserve(hash_keys.size());

	for (const auto& hash_key : hash_keys)
		if (not hash.contains(hash_key))
			values.emplace_back(std::nullopt);
		else
			values.emplace_back(hash.at(hash_key));

	return values;
}

} // namespace vanity::db
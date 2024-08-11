//
// Created by kingsli on 10/29/23.
//

#include "primitive_database.h"

namespace vanity::db {

PrimitiveDatabase::PrimitiveDatabase() = default;

PrimitiveDatabase::PrimitiveDatabase(PrimitiveDatabase &&other) noexcept
	: BaseDatabase(std::move(other)) {};

PrimitiveDatabase &PrimitiveDatabase::operator=(PrimitiveDatabase &&other) noexcept {
	BaseDatabase::operator=(std::move(other));
	return *this;
}

void PrimitiveDatabase::str_set(const key_type& key, const std::string& value) {
	m_data[key] = value;
	clear_expiry(key);
}

void PrimitiveDatabase::int_set(const key_type& key, int_t value) {
	m_data[key] = value;
	clear_expiry(key);
}

void PrimitiveDatabase::float_set(const key_type& key, float_t value) {
	m_data[key] = value;
	clear_expiry(key);
}

std::optional<int_t> PrimitiveDatabase::incr_int(const key_type &key, int_t value) {
	erase_if_expired(key);
	if (m_data.contains(key)) {
		if (std::holds_alternative<int_t>(m_data.at(key))){
			auto& val = std::get<int_t>(m_data.at(key));
			val += value;
			return val;
		}
		else
			return std::nullopt;
	}
	else {
		m_data[key] = value;
		return value;
	}
}

std::optional<float_t> PrimitiveDatabase::incr_float(const key_type &key, float_t value) {
	erase_if_expired(key);
	if (m_data.contains(key)) {
		if (std::holds_alternative<float_t>(m_data.at(key))){
			auto& val = std::get<float_t>(m_data.at(key));
			val += value;
			return val;
		}
		else
			return std::nullopt;
	}
	else {
		m_data[key] = value;
		return value;
	}
}

std::optional<int_t> PrimitiveDatabase::str_len(const key_type &key) {
	erase_if_expired(key);
	if (m_data.contains(key) and std::holds_alternative<string_t>(m_data.at(key)))
		return std::get<string_t>(m_data.at(key)).size();
	else
		return std::nullopt;
}

std::vector<std::optional<PrimitiveDatabase::data_type>> PrimitiveDatabase::many_get(const std::vector<key_type>& keys) {
	for (const auto &key: keys)
		erase_if_expired(key);

	std::vector<std::optional<data_type>> values;
	values.reserve(keys.size());
	for (const auto &key: keys)
		values.emplace_back(get(key));

	return values;
}

} // namespace vanity::db
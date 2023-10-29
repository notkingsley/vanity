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

auto PrimitiveDatabase::get(const key_type& key) -> std::optional<const data_type> {
	if (m_data.contains(key))
		return m_data.at(key);
	return std::nullopt;
}

void PrimitiveDatabase::set(const key_type& key, const data_type& value) {
	m_data[key] = value;
}

std::optional<int_t> PrimitiveDatabase::incr_int(const key_type &key, int_t value) {
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

std::optional<int_t> PrimitiveDatabase::len_str(const key_type &key) {
	if (m_data.contains(key) and std::holds_alternative<string_t>(m_data.at(key)))
		return std::get<string_t>(m_data.at(key)).size();
	else
		return std::nullopt;
}

std::vector<std::optional<PrimitiveDatabase::data_type>> PrimitiveDatabase::many_get(const std::vector<key_type>& keys) {
	std::vector<std::optional<data_type>> values;
	values.reserve(keys.size());
	for (const auto &key: keys)
		values.emplace_back(get(key));
	return values;
}

void PrimitiveDatabase::many_set(std::vector<std::pair<key_type, data_type>> pairs) {
	for (auto& pair: pairs)
		m_data.emplace(std::move(pair));
}

} // namespace vanity::db
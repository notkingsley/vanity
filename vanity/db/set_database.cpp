//
// Created by kingsli on 11/1/23.
//

#include <random>

#include "set_database.h"


namespace vanity::db {

SetDatabase::SetDatabase() = default;

SetDatabase::SetDatabase(SetDatabase &&other) noexcept
	: BaseDatabase(std::move(other)) {}

SetDatabase &SetDatabase::operator=(SetDatabase &&other) noexcept {
	BaseDatabase::operator=(std::move(other));
	return *this;
}

std::optional<size_t>
SetDatabase::set_add(const key_type &key, set_t values) {
	if (values.empty())
		return 0ull;

	if (not m_data.contains(key))
		m_data[key] = set_t{};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<set_t>(value))
		return std::nullopt;

	auto& set = std::get<set_t>(value);
	auto size = set.size();
	set.merge(values);
	return set.size() - size;
}

std::optional<set_t>
SetDatabase::set_all(const key_type &key) {
	if (not m_data.contains(key))
		return std::nullopt;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<set_t>(value))
		return std::nullopt;

	return std::get<set_t>(value);
}

std::optional<set_t>
SetDatabase::set_remove(const key_type &key, size_t count) {
	if (not m_data.contains(key))
		return std::nullopt;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<set_t>(value))
		return std::nullopt;

	auto& set = std::get<set_t>(value);
	count = std::min(count, set.size());

	static std::random_device rd;
	static std::mt19937 gen(rd());

	set_t removed;
	for (size_t i = 0; i < count; ++i) {
		auto it = set.begin();
		std::uniform_int_distribution<size_t> dis(0, set.size() - 1);
		std::advance(it, dis(gen));
		removed.insert(set.extract(it));
	}

	if (set.empty())
		m_data.erase(key);
	return removed;
}

std::optional<size_t>
SetDatabase::set_discard(const key_type &key, const set_t& values) {
	if (not m_data.contains(key))
		return 0ull;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<set_t>(value))
		return std::nullopt;

	auto& set = std::get<set_t>(value);
	auto size = set.size();
	for (const auto& v : values)
		set.erase(v);

	if (set.empty())
		m_data.erase(key);
	return size - set.size();
}

std::optional<size_t>
SetDatabase::set_len(const key_type &key) {
	if (not m_data.contains(key))
		return 0ull;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<set_t>(value))
		return std::nullopt;

	return std::get<set_t>(value).size();
}

std::optional<bool>
SetDatabase::set_contains(const key_type &key, const std::string &value) {
	if (not m_data.contains(key))
		return false;

	auto& val = m_data.at(key);
	if (not std::holds_alternative<set_t>(val))
		return std::nullopt;

	return std::get<set_t>(val).contains(value);
}

std::optional<bool>
SetDatabase::set_move(const key_type &source, const key_type &dest, const std::string &value) {
	if (not m_data.contains(source))
		return false;

	auto& source_val = m_data.at(source);
	if (not std::holds_alternative<set_t>(source_val))
		return std::nullopt;

	auto& source_set = std::get<set_t>(source_val);
	if (not source_set.contains(value))
		return false;

	if (not m_data.contains(dest))
		m_data[dest] = set_t{};

	auto& dest_val = m_data.at(dest);
	if (not std::holds_alternative<set_t>(dest_val))
		return std::nullopt;

	auto& dest_set = std::get<set_t>(dest_val);
	dest_set.insert(source_set.extract(value));

	if (source_set.empty())
		m_data.erase(source);
	return true;
}

std::optional<set_t>
SetDatabase::set_union(const std::vector<key_type> &keys) {
	if (keys.empty())
		return set_t{};

	std::vector<const set_t*> sets;
	for (const auto& key : keys) {
		if (not m_data.contains(key))
			continue;

		auto& value = m_data.at(key);
		if (not std::holds_alternative<set_t>(value))
			return std::nullopt;

		sets.push_back(&std::get<set_t>(value));
	}

	set_t result;
	for (const auto& set : sets) {
		set_t tmp = *set; // copy
		result.merge(tmp);
	}

	return result;
}

std::optional<size_t>
SetDatabase::set_union_into(const key_type &dest, const std::vector<key_type> &keys) {
	auto result = set_union(keys);
	if (not result)
		return std::nullopt;

	auto size = result->size();
	m_data.emplace(dest, std::move(*result));
	return size;
}

std::optional<size_t>
SetDatabase::set_union_len(const std::vector<key_type> &keys) {
	auto result = set_union(keys);
	if (not result)
		return std::nullopt;

	return result->size();
}

std::optional<set_t>
SetDatabase::set_intersection(const std::vector<key_type> &keys) {
	if (keys.empty())
		return set_t{};

	std::vector<const set_t*> sets;
	const set_t* smallest = nullptr;
	for (const auto& key : keys) {
		if (not m_data.contains(key))
			return set_t{};

		auto& value = m_data.at(key);
		if (not std::holds_alternative<set_t>(value))
			return std::nullopt;

		const auto& set = std::get<set_t>(value);
		sets.push_back(&set);
		if (not smallest or set.size() < smallest->size())
			smallest = &set;
	}

	set_t result;
	for (const auto& value : *smallest) {
		bool common = true;
		for (const auto& set : sets) {
			if (&set == &smallest)
				continue;

			if (not set->contains(value)) {
				common = false;
				break;
			}
		}

		if (common)
			result.insert(value);
	}

	return result;
}

std::optional<size_t>
SetDatabase::set_intersection_into(const key_type &dest, const std::vector<key_type> &keys) {
	auto result = set_intersection(keys);
	if (not result)
		return std::nullopt;

	auto size = result->size();
	m_data.emplace(dest, std::move(*result));
	return size;
}

std::optional<size_t>
SetDatabase::set_intersection_len(const std::vector<key_type> &keys) {
	auto result = set_intersection(keys);
	if (not result)
		return std::nullopt;

	return result->size();
}

std::optional<set_t>
SetDatabase::set_difference(const key_type &key1, const key_type &key2) {
	if (not m_data.contains(key1) or not m_data.contains(key2))
		return set_t{};

	auto& value1 = m_data.at(key1);
	auto& value2 = m_data.at(key2);
	if (not std::holds_alternative<set_t>(value1) or not std::holds_alternative<set_t>(value2))
		return std::nullopt;

	const auto& set1 = std::get<set_t>(value1);
	const auto& set2 = std::get<set_t>(value2);
	set_t result;
	for (const auto& value : set1) {
		if (not set2.contains(value))
			result.insert(value);
	}

	return result;
}

std::optional<size_t>
SetDatabase::set_difference_into(const key_type &dest, const key_type &key1, const key_type &key2) {
	auto result = set_difference(key1, key2);
	if (not result)
		return std::nullopt;

	auto size = result->size();
	m_data.emplace(dest, std::move(*result));
	return size;
}

std::optional<size_t>
SetDatabase::set_difference_len(const key_type &key1, const key_type &key2) {
	auto result = set_difference(key1, key2);
	if (not result)
		return std::nullopt;

	return result->size();
}

} // namespace vanity::db
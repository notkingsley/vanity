//
// Created by kingsli on 10/29/23.
//

#include "list_database.h"

namespace vanity::db {

ListDatabase::ListDatabase() = default;

ListDatabase::ListDatabase(ListDatabase &&other) noexcept
	: BaseDatabase(std::move(other)) {}

ListDatabase &ListDatabase::operator=(ListDatabase &&other) noexcept {
	BaseDatabase::operator=(std::move(other));
	return *this;
}

std::variant<size_t, ErrorKind>
ListDatabase::list_len(const key_type &key) {
	if (not m_data.contains(key))
		return 0ull;

	auto& value = m_data.at(key);
	if (std::holds_alternative<list_t>(value))
		return std::get<list_t>(value).size();
	else
		return ErrorKind::NotList;
}

std::variant<std::string, ErrorKind>
ListDatabase::list_get(const key_type &key, int index) {
	auto it_or_error = iterator_or_error(key, index);
	if (std::holds_alternative<ErrorKind>(it_or_error))
		return std::get<ErrorKind>(it_or_error);
	return *std::get<list_t::iterator>(it_or_error);
}

std::variant<bool, ErrorKind>
ListDatabase::list_set(const key_type &key, int index, const std::string &value) {
	auto it_or_error = iterator_or_error(key, index);
	if (std::holds_alternative<ErrorKind>(it_or_error))
		return std::get<ErrorKind>(it_or_error);

	auto& it = std::get<list_t::iterator>(it_or_error);
	*it = value;
	return true;
}

std::variant<size_t, ErrorKind>
ListDatabase::list_push_left(const key_type &key, list_t values) {
	if (values.empty())
		return 0ull;

	if (not m_data.contains(key))
		m_data[key] = list_t{};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<list_t>(value))
		return ErrorKind::NotList;

	auto& list = std::get<list_t>(value);
	values.reverse();
	list.splice(list.begin(), std::move(values));
	return list.size();
}

std::variant<size_t, ErrorKind>
ListDatabase::list_push_right(const key_type &key, list_t values) {
	if (values.empty())
		return 0ull;

	if (not m_data.contains(key))
		m_data[key] = list_t{};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<list_t>(value))
		return ErrorKind::NotList;

	auto& list = std::get<list_t>(value);
	list.splice(list.end(), std::move(values));
	return list.size();
}

std::variant<list_t, ErrorKind>
ListDatabase::list_pop_left(const key_type &key, int n) {
	if (not m_data.contains(key))
		return list_t{};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<list_t>(value))
		return ErrorKind::NotList;

	auto& list = std::get<list_t>(value);
	auto it = iterator_or_end(list, n);
	if (it == list.end() and n < 0)
		return list_t{}; // overflow on negative n

	list_t result;
	result.splice(result.begin(), list, list.begin(), it);
	if (list.empty())
		m_data.erase(key);

	return result;
}

std::variant<list_t, ErrorKind>
ListDatabase::list_pop_right(const key_type &key, int n) {
	if (not m_data.contains(key))
		return list_t{};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<list_t>(value))
		return ErrorKind::NotList;

	auto& list = std::get<list_t>(value);
	auto rit = reverse_iterator_or_rend(list, n);
	if (rit == list.rend() and n < 0)
		return list_t{}; // overflow on negative n

	list_t result;
	result.splice(result.begin(), list, rit.base(), list.end());
	if (list.empty())
		m_data.erase(key);

	return result;
}

std::variant<list_t, ErrorKind>
ListDatabase::list_range(const key_type &key, int start, int end) {
	if (not m_data.contains(key))
		return list_t{};

	auto& value = m_data.at(key);
	if (not std::holds_alternative<list_t>(value))
		return ErrorKind::NotList;

	if (is_invalid_range(start, end))
		return list_t{};

	auto& list = std::get<list_t>(value);
	auto [it, end_it] = iterator_pair_inclusive(list, start, end);
	return list_t{it, end_it};
}

std::variant<size_t, ErrorKind>
ListDatabase::list_trim(const key_type &key, int start, int end) {
	if (not m_data.contains(key))
		return 0ull;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<list_t>(value))
		return ErrorKind::NotList;

	if (is_invalid_range(start, end))
		return 0ull;

	auto& list = std::get<list_t>(value);
	auto [it, end_it] = iterator_pair_inclusive(list, start, end);
	auto size = list.size();

	list.erase(list.begin(), it);
	list.erase(end_it, list.end());

	auto ret = size - list.size();
	if (list.empty())
		m_data.erase(key);

	return ret;
}

std::variant<size_t, ErrorKind>
ListDatabase::list_remove(const key_type &key, const std::string &element, int count) {
	if (not m_data.contains(key))
		return 0ull;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<list_t>(value))
		return ErrorKind::NotList;

	auto& list = std::get<list_t>(value);
	auto size = list.size();

	if (count > 0) {
		auto it = list.begin();
		auto end_it = list.end();
		for (int i = 0; i < count and it != end_it;)
			if (*it == element)
				it = list.erase(it), ++i;
			else
				++it;
	} else if (count < 0) {
		auto it = list.rbegin();
		auto end_it = list.rend();
		for (int i = 0; i > count and it != end_it;)
			if (*it == element)
				it = std::make_reverse_iterator(list.erase(it.base())), --i;
			else
				++it;
	} else {
		auto it = list.begin();
		auto end_it = list.end();
		for (; it != end_it;)
			if (*it == element)
				it = list.erase(it);
			else
				++it;
	}

	auto ret = size - list.size();
	if (list.empty())
		m_data.erase(key);

	return ret;
};

std::variant<list_t::iterator, ErrorKind>
ListDatabase::iterator_or_error(const key_type &key, int index) {
	if (not m_data.contains(key))
		return ErrorKind::OutOfRange;

	auto& value = m_data.at(key);
	if (not std::holds_alternative<list_t>(value))
		return ErrorKind::NotList;

	auto& list = std::get<list_t>(value);
	auto it = iterator_or_end(list, index);
	if (it == list.end())
		return ErrorKind::OutOfRange;
	return it;
}

list_t::iterator
ListDatabase::iterator_or_end(list_t& list, int index) {
	const int64_t size = list.size();
	int64_t idx = index;

	if (idx >= size or idx < -size)
		return list.end();

	if (idx < 0)
		idx += size;

	auto it = list.begin();
	std::advance(it, idx);
	return it;
}

list_t::reverse_iterator
ListDatabase::reverse_iterator_or_rend(list_t &list, int index) {
	const int64_t size = list.size();
	int64_t idx = index;

	if (idx >= size or idx < -size)
		return list.rend();

	if (idx < 0)
		idx += size;

	auto it = list.rbegin();
	std::advance(it, idx);
	return it;
}

std::pair<list_t::iterator, list_t::iterator>
ListDatabase::iterator_pair_inclusive(list_t &list, int start, int end) {
	auto it = iterator_or_end(list, start);
	if (it == list.end() and start < 0)
		it = list.begin(); // overflow on negative start, start at the beginning

	auto end_it = iterator_or_end(list, end);
	if (end_it == list.end() and end < 0)
		end_it = list.begin(); // overflow on negative end, empty list
	else if (end_it != list.end())
		++end_it; // end is inclusive

	return {it, end_it};
}

bool ListDatabase::is_invalid_range(int start, int end) {
	return (start > 0 and end > 0 and start > end) or (start < 0 and end < 0 and start > end);
}

} // namespace vanity::db
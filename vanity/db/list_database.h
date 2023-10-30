//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_LIST_DATABASE_H
#define VANITY_LIST_DATABASE_H

#include "base_database.h"


namespace vanity::db {

enum class ErrorKind {
	NotList,
	OutOfRange,
};

/*
 * A ListDatabase implements the functionalities responsible for
 * the database's list type
 */
class ListDatabase : public virtual BaseDatabase
{
public:
	// create a new database
	ListDatabase();

	// no copy
	ListDatabase(const ListDatabase&) = delete;
	ListDatabase& operator=(const ListDatabase&) = delete;

	// move constructor
	ListDatabase(ListDatabase&& other) noexcept;

	// move assignment
	ListDatabase& operator=(ListDatabase&& other) noexcept;

	// get the length of a list key
	// returns the length, 0 if value does not exist or is empty,
	// or ErrorKind::NotList if the value exists and is not a list
	std::variant<size_t, ErrorKind> list_len(const key_type &key);

	// get the value for a list key at a given index
	// returns the value, or ErrorKind::NotList if the value is not a list
	// or ErrorKind::OutOfRange if the index is out of range or if value does not exist
	std::variant<std::string, ErrorKind> list_get(const key_type &key, int64_t index);

	// set the value for a list key at a given index
	// returns true if the value was set, or ErrorKind::NotList if the value is not a list
	// or ErrorKind::OutOfRange if the index is out of range or if value does not exist
	std::variant<bool, ErrorKind> list_set(const key_type &key, int64_t index, const std::string& value);

	// push values to the top of a list
	// returns the new length, or ErrorKind::NotList if the value is not a list
	std::variant<size_t, ErrorKind> list_push_left(const key_type &key, list_t values);

	// push values to the bottom of a list
	// returns the new length, or ErrorKind::NotList if the value is not a list
	std::variant<size_t, ErrorKind> list_push_right(const key_type &key, list_t values);

	// pop n value from the top of a list
	// returns the values or ErrorKind::NotList if the value is not a list
	// if the index is positive and out of range, returns the values up to the end of the list
	// if the index is negative and out of range, returns an empty list
	std::variant<list_t, ErrorKind> list_pop_left(const key_type &key, int64_t n);

	// pop n values from the bottom of a list
	// returns the values, or ErrorKind::NotList if the value is not a list
	// if the index is positive and out of range, returns the values up to the end of the list
	// if the index is negative and out of range, returns an empty list
	std::variant<list_t, ErrorKind> list_pop_right(const key_type &key, int64_t n);

	// get the value for a range of a list key inclusively
	// returns the values, or ErrorKind::NotList if the value is not a list
	// returns an empty list if the range is out of bounds or invalid
	std::variant<list_t, ErrorKind> list_range(const key_type &key, int64_t start, int64_t end);

	// trim the list stored at key, so that it will
	// contain only the specified range of elements (inclusively)
	// returns the number of trimmed elements
	// or ErrorKind::NotList if the value is not a list
	std::variant<size_t, ErrorKind> list_trim(const key_type &key, int64_t start, int64_t end);

	// remove a number of elements equal to count from the list stored at key
	// that hold the value element
	// returns the number of removed elements, or ErrorKind::NotList if the value is not a list
	// removes from the end if count is negative or all elements if count is 0
	std::variant<size_t, ErrorKind> list_remove(const key_type &key, const std::string& element, int64_t count);

private:
	// get the iterator for a list key at a given index
	// returns the iterator, or ErrorKind::NotList if the value is not a list
	// or ErrorKind::OutOfRange if the index is out of range or if value does not exist
	// index can be negative to get the element from the end of the list
	std::variant<list_t::iterator, ErrorKind> iterator_or_error(const key_type &key, int64_t index);

	// get the iterator for a given list at a given index
	// returns the iterator, or the end iterator if the index is out of range
	// index can be negative to get the element from the end of the list
	static list_t::iterator iterator_or_end(list_t &list, int64_t index);

	// get the reverse iterator for a given list at a given index
	// returns the iterator, or the rend iterator if the index is out of range
	// index can be negative to get the element from the end of the list
	static list_t::reverse_iterator reverse_iterator_or_rend(list_t &list, int64_t index);

	// get a pair of iterators for a given list at given indexes inclusively
	static std::pair<list_t::iterator, list_t::iterator> iterator_pair_inclusive(list_t &list, int64_t start, int64_t end);

	// check if a pair of integers form an invalid range
	static bool is_invalid_range(int64_t start, int64_t end);
};

} // namespace vanity::db

#endif //VANITY_LIST_DATABASE_H

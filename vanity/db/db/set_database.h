//
// Created by kingsli on 11/1/23.
//

#ifndef VANITY_SET_DATABASE_H
#define VANITY_SET_DATABASE_H

#include "base_database.h"

namespace vanity::db {

/*
 * A SetDatabase is a database that handles set operations
 */
class SetDatabase : public virtual BaseDatabase
{
	public:
	// create a new set database
	SetDatabase();

	// no copy
	SetDatabase(const SetDatabase&) = delete;
	SetDatabase& operator=(const SetDatabase&) = delete;

	// move constructor
	SetDatabase(SetDatabase&& other) noexcept;

	// move assignment
	SetDatabase& operator=(SetDatabase&& other) noexcept;

	// add elements to a set key
	// creates the set if it does not exist
	// returns the number of elements added,
	// or std::nullopt if the value is not a set
	std::optional<size_t> set_add(const key_type &key, set_t values);

	// get all elements from a set key
	// returns the elements,
	// or std::nullopt if the value is not a set
	std::optional<set_t> set_all(const key_type &key);

	// randomly remove elements from a set key
	// returns the elements removed,
	// or std::nullopt if the value is not a set
	std::optional<set_t> set_remove(const key_type &key, size_t count);

	// discard elements from a set key
	// return the number of elements discarded,
	// or std::nullopt if the value is not a set
	std::optional<size_t> set_discard(const key_type &key, const set_t& values);

	// get the length of a set key
	// returns the length, 0 if value does not exist or is empty,
	// or std::nullopt if the value exists and is not a set
	std::optional<size_t> set_len(const key_type &key);

	// check if a set key contains a value
	// returns true if the value exists,
	// or std::nullopt if the value is not a set
	std::optional<bool> set_contains(const key_type &key, const std::string& value);

	// atomically move an element from one set to another
	// return false if no operation was performed, true if it was
	// return std::nullopt if the value is not a set
	std::optional<bool> set_move(const key_type& source, const key_type& dest, const std::string& value);

	// get the union of a list of sets
	// returns the union set
	// or std::nullopt if one of the keys is not a set
	std::optional<set_t> set_union(const std::vector<key_type>& keys);

	// compute the union of a list of sets and store it in a key
	// returns the number of elements in the resulting set
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_union_into(const key_type& dest, const std::vector<key_type>& keys);

	// get the length of the union of a list of sets
	// returns the length of the union
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_union_len(const std::vector<key_type>& keys);

	// get the intersection of a list of sets
	// returns the intersection set
	// or std::nullopt if one of the keys is not a set
	std::optional<set_t> set_intersection(const std::vector<key_type>& keys);

	// compute the intersection of a list of sets and store it in a key
	// returns the number of elements in the resulting set
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_intersection_into(const key_type& dest, const std::vector<key_type>& keys);

	// get the length of the intersection of a list of sets
	// returns the length of the intersection
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_intersection_len(const std::vector<key_type>& keys);

	// get the difference of two sets
	// returns the difference set
	// or std::nullopt if one of the keys is not a set
	std::optional<set_t> set_difference(const key_type& key1, const key_type& key2);

	// compute the difference of two sets and store it in a key
	// returns the number of elements in the resulting set
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_difference_into(const key_type& dest, const key_type& key1, const key_type& key2);

	// get the length of the difference of two sets
	// returns the length of the difference
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_difference_len(const key_type& key1, const key_type& key2);
};

} // namespace vanity::db

#endif //VANITY_SET_DATABASE_H

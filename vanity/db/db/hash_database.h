//
// Created by kingsli on 11/7/23.
//

#ifndef VANITY_HASH_DATABASE_H
#define VANITY_HASH_DATABASE_H

#include "base_database.h"

namespace vanity::db {

enum class HashError {
	NotHash,
	BadKey,
};

/*
 * A HashDatabase is a database that handles hash operations
 */
class HashDatabase : public virtual BaseDatabase
{
public:
	// create a new hash database
	HashDatabase();

	// no copy
	HashDatabase(const HashDatabase&) = delete;
	HashDatabase& operator=(const HashDatabase&) = delete;

	// move constructor
	HashDatabase(HashDatabase&& other) noexcept;

	// move assignment
	HashDatabase& operator=(HashDatabase&& other) noexcept;

	// set a hash key
	// overwrites the content's key if it already exists
	void hash_set(const key_type &key, hash_t values);

	// get all elements from a hash key
	// returns the elements,
	// or HashError::NotHash if the value is not a hash
	std::variant<hash_t, HashError> hash_all(const key_type &key);

	// get the value of a hash key
	// returns the value,
	// or HashError::NotHash if the value is not a hash
	// or HashError::BadKey if key does not exist
	// or hash_key does not exist in the hash
	std::variant<string_t, HashError> hash_get(const key_type &key, const string_t& hash_key);

	// check if a hash key contains a value
	// returns true if the value exists,
	// or HashError::NotHash if the value is not a hash
	std::variant<bool, HashError> hash_contains(const key_type &key, const string_t& hash_key);

	// get the length of a hash
	// returns the length, 0 if value does not exist or is empty,
	// or HashError::NotHash if the value exists and is not a hash
	std::variant<size_t, HashError> hash_len(const key_type &key);

	// get the length of a hash key
	// returns the length, 0 if value does not exist or is empty,
	// or HashError::NotHash if the value exists and is not a hash
	// or HashError::BadKey if the hash_key does not exist in the hash
	std::variant<size_t, HashError> hash_key_len(const key_type &key, const string_t& hash_key);

	// remove elements from a hash key
	// return the number of elements removed,
	// or HashError::NotHash if the value is not a hash
	std::variant<size_t, HashError> hash_remove(const key_type &key, const std::vector<string_t>& hash_keys);

	// get all keys from a hash key
	// returns the keys,
	// or HashError::NotHash if the value is not a hash
	std::variant<std::vector<string_t>, HashError> hash_keys(const key_type &key);

	// get all values from a hash key
	// returns the values,
	// or HashError::NotHash if the value is not a hash
	std::variant<std::vector<string_t>, HashError> hash_values(const key_type &key);

	// update the contents of a hash
	// returns the number of elements added,
	// or HashError::NotHash if the value is not a hash
	std::variant<size_t, HashError> hash_update(const key_type &key, hash_t values);

	// get multiple values from a hash key
	// returns the values,
	// or HashError::NotHash if the value is not a hash
	std::variant<std::vector<std::optional<string_t>>, HashError>
	hash_many_get(const key_type &key, const std::vector<string_t>& hash_keys);
};

} // namespace vanity::db

#endif //VANITY_HASH_DATABASE_H

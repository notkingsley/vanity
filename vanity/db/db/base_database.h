//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_BASE_DATABASE_H
#define VANITY_BASE_DATABASE_H

#include "expiry_database.h"

namespace vanity::db {

/*
 * A BaseDatabase is the base class for all databases
 */
class BaseDatabase : public ExpiryDatabase
{
public:
	// create a database
	BaseDatabase() = default;

	// destroy the database
	~BaseDatabase() = default;

	// no copy
	BaseDatabase(const BaseDatabase&) = delete;
	BaseDatabase& operator=(const BaseDatabase&) = delete;

	// move constructor
	BaseDatabase(BaseDatabase&& other) noexcept = default;
	BaseDatabase& operator=(BaseDatabase&& other) noexcept = default;

	// reset the database
	void reset();

	// check if the key exists
	bool has(const key_type& key);

	// get the value for a key
	std::optional<const data_type> get(const key_type& key);

	// get the type of key as an index
	std::optional<int> type(const key_type& key);

	// delete the value for a key
	bool del(const key_type& key);

	// get all keys in this database
	std::vector<key_type> keys();

	// copy a value from one key to another
	// returns true if the value was copied
	// false if the value was not copied
	bool copy_to(const key_type& from, const key_type& to);

	// move a value from one key to another
	// returns true if the value was moved
	// false if the value was not moved
	bool move_to(const key_type& from, const key_type& to);

	// copy a value from one key to another in another database
	// returns true if the value was copied
	// false if the value was not copied
	bool copy_to_db(const key_type& from, BaseDatabase& to);

	// move a value from one key to another in another database
	// returns true if the value was moved
	// false if the value was not moved
	bool move_to_db(const key_type& from, BaseDatabase& to);
};

} // namespace vanity::db

#endif //VANITY_BASE_DATABASE_H

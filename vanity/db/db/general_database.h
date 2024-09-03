//
// Created by kingsli on 9/3/24.
//

#ifndef VANITY_GENERAL_DATABASE_H
#define VANITY_GENERAL_DATABASE_H

#include "base_database.h"

namespace vanity::db {

/*
 * A GeneralDatabase is a database that handles general operations
 * (e.g. copy, move that are not specific to a type)
 */
class GeneralDatabase : public virtual BaseDatabase
{
public:
	// create a new general database
	GeneralDatabase();

	// no copy
	GeneralDatabase(const GeneralDatabase&) = delete;
	GeneralDatabase& operator=(const GeneralDatabase&) = delete;

	// move constructor
	GeneralDatabase(GeneralDatabase&& other) noexcept;

	// move assignment
	GeneralDatabase& operator=(GeneralDatabase&& other) noexcept;

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
	bool copy_to_db(const key_type& from, GeneralDatabase& to);

	// move a value from one key to another in another database
	// returns true if the value was moved
	// false if the value was not moved
	bool move_to_db(const key_type& from, GeneralDatabase& to);
};

} // namespace vanity::db

#endif //VANITY_GENERAL_DATABASE_H

//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_BASE_DATABASE_H
#define VANITY_BASE_DATABASE_H

#include "base_map.h"

namespace vanity::db {

/*
 * A BaseDatabase is the base class for all databases
 */
class BaseDatabase : public BaseMap
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
	bool has(const key_type& key) const;

	// get the value for a key
	std::optional<const data_type> get(const key_type& key);

	// get the type of key as an index
	std::optional<int> type(const key_type& key);

	// delete the value for a key
	bool del(const key_type& key);
};

} // namespace vanity::db

#endif //VANITY_BASE_DATABASE_H

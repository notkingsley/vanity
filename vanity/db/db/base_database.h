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
};

} // namespace vanity::db

#endif //VANITY_BASE_DATABASE_H

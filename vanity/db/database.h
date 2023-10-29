//
// Created by kingsli on 9/11/23.
//

#ifndef VANITY_DATABASE_H
#define VANITY_DATABASE_H

#include "primitive_database.h"
#include "exceptions.h"


namespace vanity::db {

/*
 * A Database is an abstraction for a key value database
 */
class Database : public virtual PrimitiveDatabase
{
public:
	// create a new database
	Database() = default;

	// no copy
	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	// move constructor
	Database(Database&& other) noexcept;

	// move assignment
	Database& operator=(Database&& other) noexcept;

	// persist the database to a file stream
	void persist(std::ofstream &out) const;

	// load the database from a file stream
	static Database from(std::ifstream &in);
};

} // namespace vanity::db

#endif //VANITY_DATABASE_H

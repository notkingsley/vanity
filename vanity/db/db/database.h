//
// Created by kingsli on 9/11/23.
//

#ifndef VANITY_DATABASE_H
#define VANITY_DATABASE_H

#include "hash_database.h"
#include "primitive_database.h"
#include "list_database.h"
#include "set_database.h"


namespace vanity::db {

/*
 * A Database is an abstraction for a key value database
 */
class Database:
	public HashDatabase,
	public PrimitiveDatabase,
	public ListDatabase,
	public SetDatabase
{
protected:
	using m_index_type = uint;

	// the index of this database
	m_index_type m_index = 0;

public:
	// create a new database
	explicit Database(m_index_type index);

	// no copy
	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	// move constructor
	Database(Database&& other) noexcept;

	// move assignment
	Database& operator=(Database&& other) noexcept;

	// persist the database to a file stream
	void persist(std::ofstream &out);

	// load the database from a file stream
	static Database from(std::ifstream &in);
};

} // namespace vanity::db

#endif //VANITY_DATABASE_H

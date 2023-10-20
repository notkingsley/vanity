//
// Created by kingsli on 9/11/23.
//

#ifndef VANITY_DATABASE_H
#define VANITY_DATABASE_H

#include <fstream>
#include <string>
#include <exception>
#include <unordered_map>
#include <variant>

#include "exceptions.h"


namespace vanity::db {

using string_t = std::string;
using int_t = int64_t;
using float_t = double;

/*
 * A Database is an abstraction for a key value database
 */
class Database
{
public:
	using this_type = Database;
	using key_type = string_t;
	using mapped_type = std::variant<string_t, int_t, float_t>;

	// create a key value database
	Database() = default;

	// destroy the key value database
	~Database() = default;

	// no copy
	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	// move constructor
	Database(Database&& other) noexcept = default;
	Database& operator=(Database&& other) noexcept = default;

	// persist the database to a file stream
	void persist(std::ofstream &out) const;

	// load the database from a file stream
	static this_type from(std::ifstream &in);

	// reset the database
	void reset();

	// check if the key exists
	bool has(const key_type& key) const;

	// get the value for a key
	const mapped_type& get(const key_type& key);

	// set the value for a key
	void set(const key_type& key, const mapped_type& value);

	// delete the value for a key
	bool del(const key_type& key);

private:
	// the key value store
	std::unordered_map<key_type, mapped_type> m_data;
};

} // namespace vanity::db

#endif //VANITY_DATABASE_H

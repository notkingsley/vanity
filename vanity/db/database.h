//
// Created by kingsli on 9/11/23.
//

#ifndef VANITY_DATABASE_H
#define VANITY_DATABASE_H

#include <exception>
#include <fstream>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

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
	using key_type = string_t;
	using data_type = std::variant<string_t, int_t, float_t>;

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
	static Database from(std::ifstream &in);

	// reset the database
	void reset();

	// check if the key exists
	bool has(const key_type& key) const;

	// get the value for a key
	std::optional<const data_type> get(const key_type& key);

	// set the value for a key
	void set(const key_type& key, const data_type& value);

	// delete the value for a key
	bool del(const key_type& key);

	// get the type of key as an index
	std::optional<int> type(const key_type& key);

	// increment the value for an integer key, or set it to 0 if it doesn't exist
	// returns the new value, or std::nullopt if the value is not an integer
	std::optional<int_t> incr_int(const Database::key_type &key, int_t value);

	// increment the value for a float key, or set it to 0 if it doesn't exist
	// returns the new value, or std::nullopt if the value is not a float
	std::optional<float_t> incr_float(const Database::key_type &key, float_t value);

	// get the length of a string key
	// returns the length, or std::nullopt
	// if the value does not exist or is not a string
	std::optional<int_t> len_str(const Database::key_type &key);

	// get the value for many keys
	std::vector<std::optional<data_type>> many_get(const std::vector<key_type>& keys);

	// set the value for many keys
	void many_set(std::vector<std::pair<key_type, data_type>> pairs);

private:
	// the key value store
	std::unordered_map<key_type, data_type> m_data;
};

using db_key_type = Database::key_type;

using db_data_type = Database::data_type;

using db_pair_type = std::pair<db_key_type , db_data_type>;

template<size_t I>
using db_index_t = typename std::variant_alternative_t<I, db_data_type>;

} // namespace vanity::db

#endif //VANITY_DATABASE_H

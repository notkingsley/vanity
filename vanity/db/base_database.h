//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_BASE_DATABASE_H
#define VANITY_BASE_DATABASE_H

#include <exception>
#include <fstream>
#include <list>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

namespace vanity::db {

using string_t = std::string;
using int_t = int64_t;
using float_t = double;

using list_t = std::list<string_t>;

/*
 * A BaseDatabase is the base class for all databases
 */
class BaseDatabase
{
public:
	using key_type = string_t;
	using data_type = std::variant<string_t, int_t, float_t, list_t>;

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

	// get the type of key as an index
	std::optional<int> type(const key_type& key);

	// delete the value for a key
	bool del(const key_type& key);

protected:
	// the key value store
	std::unordered_map<key_type, data_type> m_data;

};
using db_key_type = BaseDatabase::key_type;

using db_data_type = BaseDatabase::data_type;

using db_pair_type = std::pair<db_key_type , db_data_type>;

template<size_t I>
using db_index_t = typename std::variant_alternative_t<I, db_data_type>;

} // namespace vanity::db

#endif //VANITY_BASE_DATABASE_H

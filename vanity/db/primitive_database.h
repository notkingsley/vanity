//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_PRIMITIVE_DATABASE_H
#define VANITY_PRIMITIVE_DATABASE_H

#include "base_database.h"


namespace vanity::db {

/*
 * A PrimitiveDatabase implements the functionalities responsible for
 * the database's primitive type
 */
class PrimitiveDatabase : public virtual BaseDatabase
{
public:
	// create a new database
	PrimitiveDatabase();

	// no copy
	PrimitiveDatabase(const PrimitiveDatabase&) = delete;
	PrimitiveDatabase& operator=(const PrimitiveDatabase&) = delete;

	// move constructor
	PrimitiveDatabase(PrimitiveDatabase&& other) noexcept;

	// move assignment
	PrimitiveDatabase& operator=(PrimitiveDatabase&& other) noexcept;

	// get the value for a key
	std::optional<const data_type> get(const key_type& key);

	// set the value for a key
	void set(const key_type& key, const data_type& value);

	// increment the value for an integer key, or set it to 0 if it doesn't exist
	// returns the new value, or std::nullopt if the value is not an integer
	std::optional<int_t> incr_int(const key_type &key, int_t value);

	// increment the value for a float key, or set it to 0 if it doesn't exist
	// returns the new value, or std::nullopt if the value is not a float
	std::optional<float_t> incr_float(const key_type &key, float_t value);

	// get the length of a string key
	// returns the length, or std::nullopt
	// if the value does not exist or is not a string
	std::optional<int_t> str_len(const key_type &key);

	// get the value for many keys
	std::vector<std::optional<data_type>> many_get(const std::vector<key_type>& keys);

	// set the value for many keys
	void many_set(std::vector<std::pair<key_type, data_type>> pairs);
};

} // namespace vanity::db

#endif //VANITY_PRIMITIVE_DATABASE_H

//
// Created by kingsli on 11/22/23.
//

#ifndef VANITY_EXPIRY_DATABASE_H
#define VANITY_EXPIRY_DATABASE_H

#include "base_map.h"

namespace vanity::db {

/*
 * An ExpiryDatabase is a database that allows for expiry of keys
 */
class ExpiryDatabase: public BaseMap
{
protected:
	// the expiry times for the keys
	std::unordered_map<key_type, time_t> m_expiry_times;

	// maximum number of keys to sample for shallow purge
	static constexpr size_t M_MAX_SAMPLE_SIZE = 100;

	// minimum percentage of keys that must be expired
	// for shallow purge to stop
	static constexpr double M_MIN_EXPIRED_PERCENTAGE = 0.25;

public:
	// create a new database
	ExpiryDatabase();

	// no copy
	ExpiryDatabase(const ExpiryDatabase &) = delete;
	ExpiryDatabase &operator=(const ExpiryDatabase &) = delete;

	// move constructor
	ExpiryDatabase(ExpiryDatabase &&other) noexcept = default;

	// move assignment
	ExpiryDatabase &operator=(ExpiryDatabase &&other) noexcept = default;

	// check if the key is is_expired
	bool is_expired(const key_type &key);

	// delete key if it is expired
	// this should be called before every operation
	// on a key
	// returns true if the key was deleted, false otherwise
	bool erase_if_expired(const key_type &key);

	// reset/clear the expiry time for a key
	// this should be called after every operation
	// that sets or resets the value for a key
	void clear_expiry(const key_type &key);

	// set the expiry time for a key
	void set_expiry(const key_type &key, time_t expiry_time);

	// get the expiry time for a key
	// returns std::nullopt if the key has no expiry time
	std::optional<time_t> get_expiry(const key_type &key);

	// clear all expiry times
	void clear_all_expiry();
	
	// shallow purge expired keys
	// this samples 100 of the keys, and deletes
	// all expired keys in the sample, then repeats
	// till < 25% of sampled keys are expired
	// this is a shallow purge, and is not guaranteed 
	// to delete all expired keys in the database 
	// this is intended to be called periodically, but
	// may not be particularly useful since the
	// sampling process is linear in the number of keys anyway
	void shallow_purge();
	
	// deep purge expired keys
	// this deletes all expired keys in the database
	// this is a deep purge, and is guaranteed to delete
	// all expired keys in the database
	// should not be called often
	void deep_purge();
};

} // namespace vanity::db

#endif //VANITY_EXPIRY_DATABASE_H

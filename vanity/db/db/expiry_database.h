//
// Created by kingsli on 11/22/23.
//

#ifndef VANITY_EXPIRY_DATABASE_H
#define VANITY_EXPIRY_DATABASE_H

#include <functional>

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

private:
	// whether key expiring should actually happen
	// this is useful for - and should be turned off when
	// - recovery by WAL redo is in progress
	bool m_expiry_enabled = true;

	// maximum number of keys to sample for shallow purge
	static constexpr size_t M_MAX_SAMPLE_SIZE = 100;

	// minimum percentage of keys that must be expired
	// for shallow purge to stop
	static constexpr double M_MIN_EXPIRED_PERCENTAGE = 0.25;

	// expire a key
	void expire(const key_type& key);

	// perform the actual key erasure
	void _do_expire(const key_type& key);

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

	// enable or disable global expiry for this database
	// when enabled, expiry works as normal and keys are expired
	// when their expiry time is met
	// when disabled, keys persist indefinitely - no matter what
	// the expiry times say - unless it is re-enabled or force_expire()
	// is used
	void expiry_enabled(bool enable);

	// manually trigger a key to be expired
	// this bypasses all checks and will remove the key
	// even if the expiry time isn't passed or if
	// expiry has been otherwise disabled
	// will not trigger the on_expire callback
	void force_expire(const key_type& key);

	// function called before a key is expired
	virtual void pre_expire(const key_type& key) { }
};

} // namespace vanity::db

#endif //VANITY_EXPIRY_DATABASE_H

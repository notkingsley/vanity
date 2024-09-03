//
// Created by kingsli on 11/11/23.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#ifndef VANITY_LOCKED_DATABASE_H
#define VANITY_LOCKED_DATABASE_H

#include <mutex>

#include "database.h"
#include "wal/write_ahead_logger.h"


namespace vanity::db {

/*
 * A database that is locked by a mutex.
 */
class LockedDatabase : public Database
{
private:
	using lock_type = std::recursive_mutex;

	// the mutex
	lock_type m_mutex;

public:
	// the logger type
	using logger_type = wal::WriteAheadLogger;

private:
	// the Write Ahead Logger
	logger_type& m_wal_logger;

	using get_db_func_t = const std::function<LockedDatabase & (m_index_type)>&;

	// log an operation to the WAL
	// assumes the lock is already acquired
	template<typename ...Args>
	inline void wal_log(trn_id_t trn_id, db_op_t op, const Args &... args);

public:
	// create a locked database
	LockedDatabase(m_index_type index, wal::WriteAheadLogger& wal_logger);

	// no copy
	LockedDatabase(const LockedDatabase&) = delete;

	LockedDatabase& operator=(const LockedDatabase&) = delete;

	// move construct a locked database from a database
	explicit LockedDatabase(Database&& other, logger_type& wal_logger) noexcept;

	// move assign a locked database from a database
	LockedDatabase& operator=(Database&& other) noexcept;

	// load the database from a file stream
	static LockedDatabase from(std::ifstream &in, logger_type& wal_logger);

	// persist the database to a file stream
	void persist(std::ofstream &out);


	// acquire the lock
	std::lock_guard<lock_type> lock();

	// return a reference to the mutex
	lock_type& mutex();


	// redo a database operation
	void wal_redo_db_op(trn_id_t trn_id, db_op_t op, std::ifstream &in, get_db_func_t& get_db);

	// redo a key expiry operation
	void wal_redo_expiry(const key_type &key);

	// begin a transaction
	trn_id_t begin(trn_id_t trn);

	// commit a transaction
	void commit(trn_id_t trn);

	// discard a transaction
	void discard(trn_id_t trn);


	// reset the database
	void reset(trn_id_t trn_id);

	// check if the key exists
	bool has(trn_id_t trn_id, const key_type& key);

	// delete the value for a key
	bool del(trn_id_t trn_id, const key_type& key);

	// get the value for a key
	std::optional<data_type> get(trn_id_t trn_id, const key_type& key);

	// get the type of key as an index
	std::optional<int> type(trn_id_t trn_id, const key_type& key);

	// get all keys in this database
	std::vector<key_type> keys(trn_id_t trn_id);

	// copy a value from one key to another
	// returns true if the value was copied
	// false if the value was not copied
	bool copy_to(trn_id_t trn_id, const key_type& from, const key_type& to);

	// move a value from one key to another
	// returns true if the value was moved
	// false if the value was not moved
	bool move_to(trn_id_t trn_id, const key_type& from, const key_type& to);

	// copy a value from one key to another in another database
	// returns true if the value was copied
	// false if the value was not copied
	bool copy_to_db(trn_id_t trn_id, const key_type& from, LockedDatabase& to);

	// move a value from one key to another in another database
	// returns true if the value was moved
	// false if the value was not moved
	bool move_to_db(trn_id_t trn_id, const key_type& from, LockedDatabase& to);


	// reset/clear the expiry time for a key
	// this should be called after every operation
	// that sets or resets the value for a key
	void clear_expiry(trn_id_t trn_id, const key_type &key);

	// set the expiry time for a key
	void set_expiry(trn_id_t trn_id, const key_type &key, time_t expiry_time);

	// get the expiry time for a key
	// returns std::nullopt if the key has no expiry time
	std::optional<time_t> get_expiry(trn_id_t trn_id, const key_type &key);

	// clear all expiry times
	void clear_all_expiry(trn_id_t trn_id);

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

	// function called before a key is expired
	void pre_expire(const key_type& key) override;


	// set a string value for a key
	void str_set(trn_id_t trn_id, const key_type& key, const std::string& value);

	// set an integer value for a key
	void int_set(trn_id_t trn_id, const key_type& key, int_t value);

	// set a float value for a key
	void float_set(trn_id_t trn_id, const key_type& key, float_t value);

	// increment the value for an integer key, or set it to 0 if it doesn't exist
	// returns the new value, or std::nullopt if the value is not an integer
	std::optional<int_t> incr_int(trn_id_t trn_id, const key_type &key, int_t value);

	// increment the value for a float key, or set it to 0 if it doesn't exist
	// returns the new value, or std::nullopt if the value is not a float
	std::optional<float_t> incr_float(trn_id_t trn_id, const key_type &key, float_t value);

	// get the length of a string key
	// returns the length, or std::nullopt
	// if the value does not exist or is not a string
	std::optional<int_t> str_len(trn_id_t trn_id, const key_type &key);

	// get the value for many keys
	std::vector<std::optional<data_type>> many_get(trn_id_t trn_id, const std::vector<key_type>& keys);


	// get the length of a list key
	// returns the length, 0 if value does not exist or is empty,
	// or ListErrorKind::NotList if the value exists and is not a list
	std::variant<size_t, ListErrorKind> list_len(trn_id_t trn_id, const key_type &key);

	// get the value for a list key at a given index
	// returns the value, or ListErrorKind::NotList if the value is not a list
	// or ListErrorKind::OutOfRange if the index is out of range or if value does not exist
	std::variant<std::string, ListErrorKind> list_get(trn_id_t trn_id, const key_type &key, int64_t index);

	// set the value for a list key at a given index
	// returns true if the value was set, or ListErrorKind::NotList if the value is not a list
	// or ListErrorKind::OutOfRange if the index is out of range or if value does not exist
	std::variant<bool, ListErrorKind> list_set(trn_id_t trn_id, const key_type &key, int64_t index, const std::string& value);

	// push values to the top of a list
	// returns the new length, or ListErrorKind::NotList if the value is not a list
	std::variant<size_t, ListErrorKind> list_push_left(trn_id_t trn_id, const key_type &key, list_t values);

	// push values to the bottom of a list
	// returns the new length, or ListErrorKind::NotList if the value is not a list
	std::variant<size_t, ListErrorKind> list_push_right(trn_id_t trn_id, const key_type &key, list_t values);

	// pop n value from the top of a list
	// returns the values or ListErrorKind::NotList if the value is not a list
	// if the index is positive and out of range, returns the values up to the end of the list
	// if the index is negative and out of range, returns an empty list
	std::variant<list_t, ListErrorKind> list_pop_left(trn_id_t trn_id, const key_type &key, int64_t n);

	// pop n values from the bottom of a list
	// returns the values, or ListErrorKind::NotList if the value is not a list
	// if the index is positive and out of range, returns the values up to the end of the list
	// if the index is negative and out of range, returns an empty list
	std::variant<list_t, ListErrorKind> list_pop_right(trn_id_t trn_id, const key_type &key, int64_t n);

	// get the value for a range of a list key inclusively
	// returns the values, or ListErrorKind::NotList if the value is not a list
	// returns an empty list if the range is out of bounds or invalid
	std::variant<list_t, ListErrorKind> list_range(trn_id_t trn_id, const key_type &key, int64_t start, int64_t end);

	// trim the list stored at key, so that it will
	// contain only the specified range of elements (trn_id_t trn_id, inclusively)
	// returns the number of trimmed elements
	// or ListErrorKind::NotList if the value is not a list
	std::variant<size_t, ListErrorKind> list_trim(trn_id_t trn_id, const key_type &key, int64_t start, int64_t end);

	// remove a number of elements equal to count from the list stored at key
	// that hold the value element
	// returns the number of removed elements, or ListErrorKind::NotList if the value is not a list
	// removes from the end if count is negative or all elements if count is 0
	std::variant<size_t, ListErrorKind> list_remove(trn_id_t trn_id, const key_type &key, const std::string& element, int64_t count);


	// add elements to a set key
	// creates the set if it does not exist
	// returns the number of elements added,
	// or std::nullopt if the value is not a set
	std::optional<size_t> set_add(trn_id_t trn_id, const key_type &key, set_t values);

	// get all elements from a set key
	// returns the elements,
	// or std::nullopt if the value is not a set
	std::optional<set_t> set_all(trn_id_t trn_id, const key_type &key);

	// randomly remove elements from a set key
	// returns the elements removed,
	// or std::nullopt if the value is not a set
	std::optional<set_t> set_remove(trn_id_t trn_id, const key_type &key, size_t count);

	// discard elements from a set key
	// return the number of elements discarded,
	// or std::nullopt if the value is not a set
	std::optional<size_t> set_discard(trn_id_t trn_id, const key_type &key, const set_t& values);

	// get the length of a set key
	// returns the length, 0 if value does not exist or is empty,
	// or std::nullopt if the value exists and is not a set
	std::optional<size_t> set_len(trn_id_t trn_id, const key_type &key);

	// check if a set key contains a value
	// returns true if the value exists,
	// or std::nullopt if the value is not a set
	std::optional<bool> set_contains(trn_id_t trn_id, const key_type &key, const std::string& value);

	// atomically move an element from one set to another
	// return false if no operation was performed, true if it was
	// return std::nullopt if the value is not a set
	std::optional<bool> set_move(trn_id_t trn_id, const key_type& source, const key_type& dest, const std::string& value);

	// get the union of a list of sets
	// returns the union set
	// or std::nullopt if one of the keys is not a set
	std::optional<set_t> set_union(trn_id_t trn_id, const std::vector<key_type>& keys);

	// compute the union of a list of sets and store it in a key
	// returns the number of elements in the resulting set
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_union_into(trn_id_t trn_id, const key_type& dest, const std::vector<key_type>& keys);

	// get the length of the union of a list of sets
	// returns the length of the union
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_union_len(trn_id_t trn_id, const std::vector<key_type>& keys);

	// get the intersection of a list of sets
	// returns the intersection set
	// or std::nullopt if one of the keys is not a set
	std::optional<set_t> set_intersection(trn_id_t trn_id, const std::vector<key_type>& keys);

	// compute the intersection of a list of sets and store it in a key
	// returns the number of elements in the resulting set
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_intersection_into(trn_id_t trn_id, const key_type& dest, const std::vector<key_type>& keys);

	// get the length of the intersection of a list of sets
	// returns the length of the intersection
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_intersection_len(trn_id_t trn_id, const std::vector<key_type>& keys);

	// get the difference of two sets
	// returns the difference set
	// or std::nullopt if one of the keys is not a set
	std::optional<set_t> set_difference(trn_id_t trn_id, const key_type& key1, const key_type& key2);

	// compute the difference of two sets and store it in a key
	// returns the number of elements in the resulting set
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_difference_into(trn_id_t trn_id, const key_type& dest, const key_type& key1, const key_type& key2);

	// get the length of the difference of two sets
	// returns the length of the difference
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_difference_len(trn_id_t trn_id, const key_type& key1, const key_type& key2);


	// set a hash key
	// overwrites the content's key if it already exists
	void hash_set(trn_id_t trn_id, const key_type &key, hash_t values);

	// get all elements from a hash key
	// returns the elements,
	// or HashError::NotHash if the value is not a hash
	std::variant<hash_t, HashError> hash_all(trn_id_t trn_id, const key_type &key);

	// get the value of a hash key
	// returns the value,
	// or HashError::NotHash if the value is not a hash
	// or HashError::BadKey if key does not exist
	// or hash_key does not exist in the hash
	std::variant<string_t, HashError> hash_get(trn_id_t trn_id, const key_type &key, const string_t& hash_key);

	// check if a hash key contains a value
	// returns true if the value exists,
	// or HashError::NotHash if the value is not a hash
	std::variant<bool, HashError> hash_contains(trn_id_t trn_id, const key_type &key, const string_t& hash_key);

	// get the length of a hash
	// returns the length, 0 if value does not exist or is empty,
	// or HashError::NotHash if the value exists and is not a hash
	std::variant<size_t, HashError> hash_len(trn_id_t trn_id, const key_type &key);

	// get the length of a hash key
	// returns the length, 0 if value does not exist or is empty,
	// or HashError::NotHash if the value exists and is not a hash
	// or HashError::BadKey if the hash_key does not exist in the hash
	std::variant<size_t, HashError> hash_key_len(trn_id_t trn_id, const key_type &key, const string_t& hash_key);

	// remove elements from a hash key
	// return the number of elements removed,
	// or HashError::NotHash if the value is not a hash
	std::variant<size_t, HashError> hash_remove(trn_id_t trn_id, const key_type &key, const std::vector<string_t>& hash_keys);

	// get all keys from a hash key
	// returns the keys,
	// or HashError::NotHash if the value is not a hash
	std::variant<std::vector<string_t>, HashError> hash_keys(trn_id_t trn_id, const key_type &key);

	// get all values from a hash key
	// returns the values,
	// or HashError::NotHash if the value is not a hash
	std::variant<std::vector<string_t>, HashError> hash_values(trn_id_t trn_id, const key_type &key);

	// update the contents of a hash
	// returns the number of elements added,
	// or HashError::NotHash if the value is not a hash
	std::variant<size_t, HashError> hash_update(trn_id_t trn_id, const key_type &key, hash_t values);

	// get multiple values from a hash key
	// returns the values,
	// or HashError::NotHash if the value is not a hash
	std::variant<std::vector<std::optional<string_t>>, HashError>
	hash_many_get(trn_id_t trn_id, const key_type &key, const std::vector<string_t>& hash_keys);
};

} // namespace vanity::db

#endif //VANITY_LOCKED_DATABASE_H

#pragma clang diagnostic pop
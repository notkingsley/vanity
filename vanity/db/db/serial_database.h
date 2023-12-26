//
// Created by kingsli on 10/20/23.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#ifndef VANITY_SERIAL_DATABASE_H
#define VANITY_SERIAL_DATABASE_H

#include "database.h"
#include "utils/task_serializer.h"


namespace vanity::db {

// define the types used by the serial database
struct serial_database_types {
private:
	using key_type = Database::key_type;
	using data_type = Database::data_type;

protected:
	// parameter types
	// base database
	using reset_type = void;
	using del_type = key_type;
	using has_type = key_type;
	using type_type = key_type;
	using persist_type = std::tuple<std::ofstream&>;

	// primitive database
	using get_type = key_type;
	using set_type = std::tuple<key_type, data_type>;
	using incr_int_type = std::tuple<key_type, int_t>;
	using incr_float_type = std::tuple<key_type, float_t>;
	using str_len_type = key_type;
	using many_get_type = std::vector<key_type>;
	using many_set_type = std::vector<std::pair<key_type, data_type>>;

	// list database
	using list_len_type = key_type;
	using list_get_type = std::tuple<key_type, int64_t>;
	using list_set_type = std::tuple<key_type, int64_t, std::string>;
	using list_push_left_type = std::tuple<key_type, list_t>;
	using list_push_right_type = std::tuple<key_type, list_t>;
	using list_pop_left_type = std::tuple<key_type, int64_t>;
	using list_pop_right_type = std::tuple<key_type, int64_t>;
	using list_range_type = std::tuple<key_type, int64_t, int64_t>;
	using list_trim_type = std::tuple<key_type, int64_t, int64_t>;
	using list_remove_type = std::tuple<key_type, std::string, int64_t>;

	// set database
	using set_add_type = std::tuple<key_type, set_t>;
	using set_all_type = key_type;
	using set_remove_type = std::tuple<key_type, size_t>;
	using set_discard_type = std::tuple<key_type, const set_t&>;
	using set_len_type = key_type;
	using set_contains_type = std::tuple<key_type, std::string>;
	using set_move_type = std::tuple<key_type, key_type, std::string>;
	using set_union_type = std::vector<key_type>;
	using set_union_into_type = std::tuple<key_type, std::vector<key_type>>;
	using set_union_len_type = std::vector<key_type>;
	using set_intersection_type = std::vector<key_type>;
	using set_intersection_into_type = std::tuple<key_type, std::vector<key_type>>;
	using set_intersection_len_type = std::vector<key_type>;
	using set_difference_type = std::tuple<key_type, key_type>;
	using set_difference_into_type = std::tuple<key_type, key_type, key_type>;
	using set_difference_len_type = std::tuple<key_type, key_type>;

	// hash database
	using hash_set_type = std::tuple<key_type, std::unordered_map<std::string, std::string>>;
	using hash_all_type = key_type;
	using hash_get_type = std::tuple<key_type, std::string>;
	using hash_contains_type = std::tuple<key_type, std::string>;
	using hash_len_type = key_type;
	using hash_key_len_type = std::tuple<key_type, std::string>;
	using hash_remove_type = std::tuple<key_type, std::vector<std::string>>;
	using hash_keys_type = key_type;
	using hash_values_type = key_type;
	using hash_update_type = std::tuple<key_type, std::unordered_map<std::string, std::string>>;
	using hash_many_get_type = std::tuple<key_type, std::vector<std::string>>;


	// return types
	// base database
	using has_ret_type = bool;
	using del_ret_type = bool;
	using reset_ret_type = void;
	using type_ret_type = std::optional<int>;
	using persist_ret_type = void;

	// primitive database
	using get_ret_type = std::optional<data_type>;
	using set_ret_type = void;
	using incr_int_ret_type = std::optional<int_t>;
	using incr_float_ret_type = std::optional<float_t>;
	using str_len_ret_type = std::optional<int_t>;
	using many_get_ret_type = std::vector<std::optional<data_type>>;
	using many_set_ret_type = void;

	// list database
	using list_len_ret_type = std::variant<size_t, ListErrorKind>;
	using list_get_ret_type = std::variant<std::string, ListErrorKind>;
	using list_set_ret_type = std::variant<bool, ListErrorKind>;
	using list_push_left_ret_type = std::variant<size_t, ListErrorKind>;
	using list_push_right_ret_type = std::variant<size_t, ListErrorKind>;
	using list_pop_left_ret_type = std::variant<list_t, ListErrorKind>;
	using list_pop_right_ret_type = std::variant<list_t, ListErrorKind>;
	using list_range_ret_type = std::variant<list_t, ListErrorKind>;
	using list_trim_ret_type = std::variant<size_t, ListErrorKind>;
	using list_remove_ret_type = std::variant<size_t, ListErrorKind>;

	// set database
	using set_add_ret_type = std::optional<size_t>;
	using set_all_ret_type = std::optional<set_t>;
	using set_remove_ret_type = std::optional<set_t>;
	using set_discard_ret_type = std::optional<size_t>;
	using set_len_ret_type = std::optional<size_t>;
	using set_contains_ret_type = std::optional<bool>;
	using set_move_ret_type = std::optional<bool>;
	using set_union_ret_type = std::optional<set_t>;
	using set_union_into_ret_type = std::optional<size_t>;
	using set_union_len_ret_type = std::optional<size_t>;
	using set_intersection_ret_type = std::optional<set_t>;
	using set_intersection_into_ret_type = std::optional<size_t>;
	using set_intersection_len_ret_type = std::optional<size_t>;
	using set_difference_ret_type = std::optional<set_t>;
	using set_difference_into_ret_type = std::optional<size_t>;
	using set_difference_len_ret_type = std::optional<size_t>;

	// hash database
	using hash_set_ret_type = void;
	using hash_all_ret_type = std::variant<hash_t, HashError>;
	using hash_get_ret_type = std::variant<string_t, HashError>;
	using hash_contains_ret_type = std::variant<bool, HashError>;
	using hash_len_ret_type = std::variant<size_t, HashError>;
	using hash_key_len_ret_type = std::variant<size_t, HashError>;
	using hash_remove_ret_type = std::variant<size_t, HashError>;
	using hash_keys_ret_type = std::variant<std::vector<string_t>, HashError>;
	using hash_values_ret_type = std::variant<std::vector<string_t>, HashError>;
	using hash_update_ret_type = std::variant<size_t, HashError>;
	using hash_many_get_ret_type = std::variant<std::vector<std::optional<string_t>>, HashError>;

public:
	enum task_type {
		RESET,
		PERSIST,
		DEL,
		TYPE,

		GET,
		SET,
		HAS,
		INCR_INT,
		INCR_FLOAT,
		STR_LEN,
		MANY_GET,
		MANY_SET,

		LIST_LEN,
		LIST_GET,
		LIST_SET,
		LIST_PUSH_LEFT,
		LIST_PUSH_RIGHT,
		LIST_POP_LEFT,
		LIST_POP_RIGHT,
		LIST_RANGE,
		LIST_TRIM,
		LIST_REMOVE,

		SET_ADD,
		SET_ALL,
		SET_REMOVE,
		SET_DISCARD,
		SET_LEN,
		SET_CONTAINS,
		SET_MOVE,
		SET_UNION,
		SET_UNION_INTO,
		SET_UNION_LEN,
		SET_INTERSECTION,
		SET_INTERSECTION_INTO,
		SET_INTERSECTION_LEN,
		SET_DIFFERENCE,
		SET_DIFFERENCE_INTO,
		SET_DIFFERENCE_LEN,

		HASH_SET,
		HASH_ALL,
		HASH_GET,
		HASH_CONTAINS,
		HASH_LEN,
		HASH_KEY_LEN,
		HASH_REMOVE,
		HASH_KEYS,
		HASH_VALUES,
		HASH_UPDATE,
		HASH_MANY_GET,
	};

	using task_data_type = std::variant<
		persist_type,

		get_type,
		set_type,
		incr_int_type,
		incr_float_type,
		many_get_type,
		many_set_type,

		list_set_type,
		list_push_left_type,
		list_range_type,
		list_remove_type,

		set_add_type,
		set_remove_type,
		set_discard_type,
		set_union_into_type,
		set_difference_type,
		set_difference_into_type,

		hash_set_type,

		std::monostate
	>;
	using ret_type = std::variant<
		del_ret_type,
		type_ret_type,

		get_ret_type,
		incr_int_ret_type,
		incr_float_ret_type,
		many_get_ret_type,

		list_len_ret_type,
		list_get_ret_type,
		list_set_ret_type,
		list_pop_left_ret_type,

		set_add_ret_type,
		set_all_ret_type,
		set_contains_ret_type,

		hash_all_ret_type,
		hash_get_ret_type,
		hash_contains_ret_type,
		hash_len_ret_type,
		hash_keys_ret_type,
		hash_many_get_ret_type,

		std::monostate
	>;
};

/*
 * A SerialDatabase serializes all database operations to a single thread
 *
 * This is actually a very bad/inefficient implementation for thread safety
 * Considering using simple locks instead
 */
class SerialDatabase :
	public Database,
	private serial_database_types,
	private TaskSerializer<
		serial_database_types::task_type,
		serial_database_types::task_data_type,
		serial_database_types::ret_type
	>
{
private:
	using serial_database_types::task_data_type;
	using Database::key_type;
	using Database::data_type;

	// perform a task that was sent from another thread
	void perform(task_type task, task_data_type data, std::promise<ret_type> promise) override;

public:
	// create a serial database
	SerialDatabase() = default;

	// no copy
	SerialDatabase(const SerialDatabase&) = delete;
	SerialDatabase& operator=(const SerialDatabase&) = delete;
	
	// move construct a serial database from a database
	explicit SerialDatabase(Database&& other) noexcept;

	// move assign a serial database from a database
	SerialDatabase& operator=(Database&& other) noexcept;

	// load the database from a file stream
	static SerialDatabase from(std::ifstream &in);

	// start the database's operation thread
	void start(long max_timeout);

	// stop the database's operation thread
	void stop();


	// persist the database to a file stream
	void persist(std::ofstream &out);

	// reset the database
	void reset();

	// check if the key exists
	bool has(const key_type& key);

	// delete the value for a key
	bool del(const key_type& key);


	// get the value for a key
	std::optional<data_type> get(const key_type& key);

	// set the value for a key
	void set(const key_type& key, const db_data_type& value);

	// get the type of key as an index
	std::optional<int> type(const key_type& key);

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
	void many_set(std::vector<std::pair<db_key_type, db_data_type>> keys);


	// get the length of a list key
	// returns the length, 0 if value does not exist or is empty,
	// or ListErrorKind::NotList if the value exists and is not a list
	std::variant<size_t, ListErrorKind> list_len(const key_type &key);

	// get the value for a list key at a given index
	// returns the value, or ListErrorKind::NotList if the value is not a list
	// or ListErrorKind::OutOfRange if the index is out of range or if value does not exist
	std::variant<std::string, ListErrorKind> list_get(const key_type &key, int64_t index);

	// set the value for a list key at a given index
	// returns true if the value was set, or ListErrorKind::NotList if the value is not a list
	// or ListErrorKind::OutOfRange if the index is out of range or if value does not exist
	std::variant<bool, ListErrorKind> list_set(const key_type &key, int64_t index, const std::string& value);

	// push values to the top of a list
	// returns the new length, or ListErrorKind::NotList if the value is not a list
	std::variant<size_t, ListErrorKind> list_push_left(const key_type &key, list_t values);

	// push values to the bottom of a list
	// returns the new length, or ListErrorKind::NotList if the value is not a list
	std::variant<size_t, ListErrorKind> list_push_right(const key_type &key, list_t values);

	// pop n value from the top of a list
	// returns the values or ListErrorKind::NotList if the value is not a list
	// if the index is positive and out of range, returns the values up to the end of the list
	// if the index is negative and out of range, returns an empty list
	std::variant<list_t, ListErrorKind> list_pop_left(const key_type &key, int64_t n);

	// pop n values from the bottom of a list
	// returns the values, or ListErrorKind::NotList if the value is not a list
	// if the index is positive and out of range, returns the values up to the end of the list
	// if the index is negative and out of range, returns an empty list
	std::variant<list_t, ListErrorKind> list_pop_right(const key_type &key, int64_t n);

	// get the value for a range of a list key inclusively
	// returns the values, or ListErrorKind::NotList if the value is not a list
	// returns an empty list if the range is out of bounds or invalid
	std::variant<list_t, ListErrorKind> list_range(const key_type &key, int64_t start, int64_t end);

	// trim the list stored at key, so that it will
	// contain only the specified range of elements (inclusively)
	// returns the number of trimmed elements
	// or ListErrorKind::NotList if the value is not a list
	std::variant<size_t, ListErrorKind> list_trim(const key_type &key, int64_t start, int64_t end);

	// remove a number of elements equal to count from the list stored at key
	// that hold the value element
	// returns the number of removed elements, or ListErrorKind::NotList if the value is not a list
	// removes from the end if count is negative or all elements if count is 0
	std::variant<size_t, ListErrorKind> list_remove(const key_type &key, const std::string& element, int64_t count);


	// add elements to a set key
	// creates the set if it does not exist
	// returns the number of elements added,
	// or std::nullopt if the value is not a set
	std::optional<size_t> set_add(const key_type &key, set_t values);

	// get all elements from a set key
	// returns the elements,
	// or std::nullopt if the value is not a set
	std::optional<set_t> set_all(const key_type &key);

	// randomly remove elements from a set key
	// returns the elements removed,
	// or std::nullopt if the value is not a set
	std::optional<set_t> set_remove(const key_type &key, size_t count);

	// discard elements from a set key
	// return the number of elements discarded,
	// or std::nullopt if the value is not a set
	std::optional<size_t> set_discard(const key_type &key, const set_t& values);

	// get the length of a set key
	// returns the length, 0 if value does not exist or is empty,
	// or std::nullopt if the value exists and is not a set
	std::optional<size_t> set_len(const key_type &key);

	// check if a set key contains a value
	// returns true if the value exists,
	// or std::nullopt if the value is not a set
	std::optional<bool> set_contains(const key_type &key, const std::string& value);

	// atomically move an element from one set to another
	// return false if no operation was performed, true if it was
	// return std::nullopt if the value is not a set
	std::optional<bool> set_move(const key_type& source, const key_type& dest, const std::string& value);

	// get the union of a list of sets
	// returns the union set
	// or std::nullopt if one of the keys is not a set
	std::optional<set_t> set_union(const std::vector<key_type>& keys);

	// compute the union of a list of sets and store it in a key
	// returns the number of elements in the resulting set
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_union_into(const key_type& dest, const std::vector<key_type>& keys);

	// get the length of the union of a list of sets
	// returns the length of the union
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_union_len(const std::vector<key_type>& keys);

	// get the intersection of a list of sets
	// returns the intersection set
	// or std::nullopt if one of the keys is not a set
	std::optional<set_t> set_intersection(const std::vector<key_type>& keys);

	// compute the intersection of a list of sets and store it in a key
	// returns the number of elements in the resulting set
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_intersection_into(const key_type& dest, const std::vector<key_type>& keys);

	// get the length of the intersection of a list of sets
	// returns the length of the intersection
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_intersection_len(const std::vector<key_type>& keys);

	// get the difference of two sets
	// returns the difference set
	// or std::nullopt if one of the keys is not a set
	std::optional<set_t> set_difference(const key_type& key1, const key_type& key2);

	// compute the difference of two sets and store it in a key
	// returns the number of elements in the resulting set
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_difference_into(const key_type& dest, const key_type& key1, const key_type& key2);

	// get the length of the difference of two sets
	// returns the length of the difference
	// or std::nullopt if one of the keys is not a set
	std::optional<size_t> set_difference_len(const key_type& key1, const key_type& key2);


	// set a hash key
	// overwrites the content's key if it already exists
	void hash_set(const key_type &key, hash_t values);

	// get all elements from a hash key
	// returns the elements,
	// or HashError::NotHash if the value is not a hash
	std::variant<hash_t, HashError> hash_all(const key_type &key);

	// get the value of a hash key
	// returns the value,
	// or HashError::NotHash if the value is not a hash
	// or HashError::BadKey if key does not exist
	// or hash_key does not exist in the hash
	std::variant<string_t, HashError> hash_get(const key_type &key, const string_t& hash_key);

	// check if a hash key contains a value
	// returns true if the value exists,
	// or HashError::NotHash if the value is not a hash
	std::variant<bool, HashError> hash_contains(const key_type &key, const string_t& hash_key);

	// get the length of a hash
	// returns the length, 0 if value does not exist or is empty,
	// or HashError::NotHash if the value exists and is not a hash
	std::variant<size_t, HashError> hash_len(const key_type &key);

	// get the length of a hash key
	// returns the length, 0 if value does not exist or is empty,
	// or HashError::NotHash if the value exists and is not a hash
	// or HashError::BadKey if the hash_key does not exist in the hash
	std::variant<size_t, HashError> hash_key_len(const key_type &key, const string_t& hash_key);

	// remove elements from a hash key
	// return the number of elements removed,
	// or HashError::NotHash if the value is not a hash
	std::variant<size_t, HashError> hash_remove(const key_type &key, const std::vector<string_t>& hash_keys);

	// get all keys from a hash key
	// returns the keys,
	// or HashError::NotHash if the value is not a hash
	std::variant<std::vector<string_t>, HashError> hash_keys(const key_type &key);

	// get all values from a hash key
	// returns the values,
	// or HashError::NotHash if the value is not a hash
	std::variant<std::vector<string_t>, HashError> hash_values(const key_type &key);

	// update the contents of a hash
	// returns the number of elements added,
	// or HashError::NotHash if the value is not a hash
	std::variant<size_t, HashError> hash_update(const key_type &key, hash_t values);

	// get multiple values from a hash key
	// returns the values,
	// or HashError::NotHash if the value is not a hash
	std::variant<std::vector<std::optional<string_t>>, HashError>
	hash_many_get(const key_type &key, const std::vector<string_t>& hash_keys);

};

} // namespace vanity::db

#endif //VANITY_SERIAL_DATABASE_H

#pragma clang diagnostic pop
//
// Created by kingsli on 10/20/23.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#ifndef VANITY_SERIAL_DATABASE_H
#define VANITY_SERIAL_DATABASE_H

#include "database.h"
#include "task_serializer.h"


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
	using list_get_type = std::tuple<key_type, int>;
	using list_set_type = std::tuple<key_type, int, std::string>;
	using list_push_left_type = std::tuple<key_type, list_t>;
	using list_push_right_type = std::tuple<key_type, list_t>;
	using list_pop_left_type = std::tuple<key_type, int>;
	using list_pop_right_type = std::tuple<key_type, int>;
	using list_range_type = std::tuple<key_type, int, int>;
	using list_trim_type = std::tuple<key_type, int, int>;
	using list_remove_type = std::tuple<key_type, std::string, int>;


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
	using list_len_ret_type = std::variant<size_t, ErrorKind>;
	using list_get_ret_type = std::variant<std::string, ErrorKind>;
	using list_set_ret_type = std::variant<bool, ErrorKind>;
	using list_push_left_ret_type = std::variant<size_t, ErrorKind>;
	using list_push_right_ret_type = std::variant<size_t, ErrorKind>;
	using list_pop_left_ret_type = std::variant<list_t, ErrorKind>;
	using list_pop_right_ret_type = std::variant<list_t, ErrorKind>;
	using list_range_ret_type = std::variant<list_t, ErrorKind>;
	using list_trim_ret_type = std::variant<size_t, ErrorKind>;
	using list_remove_ret_type = std::variant<size_t, ErrorKind>;

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
		list_pop_left_type,
		list_range_type,
		list_remove_type,

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

		std::monostate
	>;
};

/*
 * A SerialDatabase serializes all database operations to a single thread
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
	// or ErrorKind::NotList if the value exists and is not a list
	std::variant<size_t, ErrorKind> list_len(const key_type &key);

	// get the value for a list key at a given index
	// returns the value, or ErrorKind::NotList if the value is not a list
	// or ErrorKind::OutOfRange if the index is out of range or if value does not exist
	std::variant<std::string, ErrorKind> list_get(const key_type &key, int index);

	// set the value for a list key at a given index
	// returns true if the value was set, or ErrorKind::NotList if the value is not a list
	// or ErrorKind::OutOfRange if the index is out of range or if value does not exist
	std::variant<bool, ErrorKind> list_set(const key_type &key, int index, const std::string& value);

	// push values to the top of a list
	// returns the new length, or ErrorKind::NotList if the value is not a list
	std::variant<size_t, ErrorKind> list_push_left(const key_type &key, list_t values);

	// push values to the bottom of a list
	// returns the new length, or ErrorKind::NotList if the value is not a list
	std::variant<size_t, ErrorKind> list_push_right(const key_type &key, list_t values);

	// pop n value from the top of a list
	// returns the values or ErrorKind::NotList if the value is not a list
	// if the index is positive and out of range, returns the values up to the end of the list
	// if the index is negative and out of range, returns an empty list
	std::variant<list_t, ErrorKind> list_pop_left(const key_type &key, int n);

	// pop n values from the bottom of a list
	// returns the values, or ErrorKind::NotList if the value is not a list
	// if the index is positive and out of range, returns the values up to the end of the list
	// if the index is negative and out of range, returns an empty list
	std::variant<list_t, ErrorKind> list_pop_right(const key_type &key, int n);

	// get the value for a range of a list key inclusively
	// returns the values, or ErrorKind::NotList if the value is not a list
	// returns an empty list if the range is out of bounds or invalid
	std::variant<list_t, ErrorKind> list_range(const key_type &key, int start, int end);

	// trim the list stored at key, so that it will
	// contain only the specified range of elements (inclusively)
	// returns the number of trimmed elements
	// or ErrorKind::NotList if the value is not a list
	std::variant<size_t, ErrorKind> list_trim(const key_type &key, int start, int end);

	// remove a number of elements equal to count from the list stored at key
	// that hold the value element
	// returns the number of removed elements, or ErrorKind::NotList if the value is not a list
	// removes from the end if count is negative or all elements if count is 0
	std::variant<size_t, ErrorKind> list_remove(const key_type &key, const std::string& element, int count);
};

} // namespace vanity::db

#endif //VANITY_SERIAL_DATABASE_H

#pragma clang diagnostic pop
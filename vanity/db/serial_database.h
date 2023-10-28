//
// Created by kingsli on 10/20/23.
//

#ifndef VANITY_SERIAL_DATABASE_H
#define VANITY_SERIAL_DATABASE_H

#include "database.h"
#include "task_serializer.h"


namespace vanity::db {

// define the types used by the serial database
struct serial_database_types {
	enum class task_type {
		GET,
		SET,
		DEL,
		TYPE,
		HAS,
		INCR_INT,
		INCR_FLOAT,
		LEN_STR,
		MANY_GET,
		MANY_SET,
		RESET,
		PERSIST,
	};

	// parameter types
	using get_type = Database::key_type;
	using set_type = std::tuple<Database::key_type, Database::data_type>;
	using del_type = Database::key_type;
	using has_type = Database::key_type;
	using type_type = Database::key_type;
	using incr_int_type = std::tuple<Database::key_type, int_t>;
	using incr_float_type = std::tuple<Database::key_type, float_t>;
	using len_str_type = Database::key_type;
	using many_get_type = std::vector<Database::key_type>;
	using many_set_type = std::vector<std::pair<Database::key_type, Database::data_type>>;
	using reset_type = void;
	using persist_type = std::tuple<std::ofstream&>;

	// return types
	using get_ret_type = std::optional<Database::data_type>;
	using set_ret_type = void;
	using del_ret_type = bool;
	using has_ret_type = bool;
	using type_ret_type = std::optional<int>;
	using incr_int_ret_type = std::optional<int_t>;
	using incr_float_ret_type = std::optional<float_t>;
	using len_str_ret_type = std::optional<int_t>;
	using many_get_ret_type = std::vector<std::optional<Database::data_type>>;
	using many_set_ret_type = void;
	using reset_ret_type = void;
	using persist_ret_type = void;


	using task_data_type = std::variant<
		get_type,
		set_type,
		incr_int_type,
		incr_float_type,
		many_get_type,
		many_set_type,
		persist_type,
		std::monostate
	>;
	using ret_type = std::variant<
		get_ret_type,
		del_ret_type,
		type_ret_type,
		incr_int_ret_type,
		incr_float_ret_type,
		many_get_ret_type,
		std::monostate
	>;
};

/*
 * A SerialDatabase serializes all database operations to a single thread
 */
class SerialDatabase :
	public virtual Database,
	private serial_database_types,
	private TaskSerializer<
		serial_database_types::task_type,
		serial_database_types::task_data_type,
		serial_database_types::ret_type
	>
{
private:
	using serial_database_types::task_data_type;

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

	// get the value for a key
	std::optional<Database::data_type> get(const key_type& key);

	// set the value for a key
	void set(const key_type& key, const db_data_type& value);

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
	std::vector<std::optional<Database::data_type>> many_get(const std::vector<key_type>& keys);

	// set the value for many keys
	void many_set(std::vector<std::pair<db_key_type, db_data_type>> keys);
};

} // namespace vanity::db

#endif //VANITY_SERIAL_DATABASE_H

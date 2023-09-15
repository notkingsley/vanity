//
// Created by kingsli on 9/11/23.
//

#ifndef VANITY_DATABASE_H
#define VANITY_DATABASE_H

#include <string>
#include <exception>
#include <unordered_map>


namespace vanity{

/*
 * A Database is an abstraction for a database
 */
class Database
{
public:
	// create a database
	Database() = default;

	// destroy the database
	virtual ~Database() = default;

	// no copy
	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	// move constructor
	Database(Database&& other) noexcept = default;
	Database& operator=(Database&& other) noexcept = default;
};


/*
 * A DatabaseException is thrown when an error occurs
 */
class DatabaseException : std::exception
{
private:
	std::string m_msg;

public:
	explicit DatabaseException(std::string msg) : m_msg{std::move(msg)} {}
	const char* what() const noexcept override { return m_msg.c_str(); }
};

/*
 * A DatabaseKeyNotFoundException is thrown when a key is not found
 */
class DatabaseKeyNotFoundException : DatabaseException {
public:
	explicit DatabaseKeyNotFoundException(std::string msg) : DatabaseException(std::move(msg)) {}
};

/*
 * A KeyValueDatabase is an abstraction for a key value database
 */
class KeyValueDatabase : public Database
{
private:
	// the key value store
	std::unordered_map<std::string, std::string> m_data;

public:
	// create a key value database
	KeyValueDatabase() = default;

	// destroy the key value database
	~KeyValueDatabase() override = default;

	// no copy
	KeyValueDatabase(const KeyValueDatabase&) = delete;
	KeyValueDatabase& operator=(const KeyValueDatabase&) = delete;

	// move constructor
	KeyValueDatabase(KeyValueDatabase&& other) noexcept = default;
	KeyValueDatabase& operator=(KeyValueDatabase&& other) noexcept = default;

	// check if the key exists
	bool has(const std::string& key) const
	{
		return m_data.contains(key);
	};

	// get the value for a key
	const std::string& get(const std::string& key)
	{
		if (has(key)) {
			return m_data.at(key);
		}
		throw DatabaseKeyNotFoundException("Key not found");
	};

	// set the value for a key
	void set(const std::string& key, const std::string& value)
	{
		m_data[key] = value;
	};

	// delete the value for a key
	bool del(const std::string& key){
		return m_data.erase(key);
	};
};

} // namespace vanity

#endif //VANITY_DATABASE_H

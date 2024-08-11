//
// Created by kingsli on 8/7/24.
//

#ifndef VANITY_WRITE_AHEAD_LOGGER_H
#define VANITY_WRITE_AHEAD_LOGGER_H

#include <filesystem>
#include <fstream>
#include <mutex>

#include "client/client.h"
#include "db/db/db_operations.h"
#include "utils/serializer.h"
#include "wal_entry_t.h"


namespace vanity::wal {

/*
 * A WriteAheadLogger logs the operations performed on a database
 */
class WriteAheadLogger
{
private:
	// the WAL file
	std::ofstream m_wal_file;

	// mutex for the WAL
	std::mutex m_wal_mutex;

	// true if the nth type in Args is
	template<int n, typename T, typename... Args>
	constexpr static bool is_of_type = std::is_same_v<T, std::tuple_element_t<n, std::tuple<Args...>>>;

	// write an entry to the wal
	template<typename ...Args>
	void wal(const Args &... args) {
		static_assert(is_of_type<0, wal_entry_t, Args...>, "First argument must be of type wal_entry_t");
		static_assert(is_of_type<1, uint, Args...>, "Second argument must be of type uint");
		static_assert(
			is_of_type<2, std::string, Args...> or is_of_type<2, db::db_op_t, Args...>,
		"Third argument must be of type std::string or db::db_op_t"
		);

		std::lock_guard lock(m_wal_mutex);
		if (not m_wal_file.is_open())
			return;

		serializer::WriteHandle writer{m_wal_file};
		(writer.write(args), ...);
		m_wal_file << std::endl;
	}

	// close the WAL
	void close_wal();

public:
	// use file for the WAL
	void wal_to(const std::filesystem::path &wal_file);

	// log a db operation that's about to happen
	template<typename ...Args>
	void wal_db_op(uint db, db::db_op_t op, const Args &... args) {
		wal(wal_entry_t::db_op, db, op, args...);
	}

	// log an expiry that's about to happen
	void wal_expiry(const std::string &key, uint db);

	// log a set_expiry operation that's about to happen
	// this is WAL-ed separately because the replay of SET_EXPIRY
	// with a WAL is dependent on a time factor,
	// violating the invariance on the principle of
	// WAL: we must arrive at exactly the same state after
	// redoing each entry in the WAL
	void wal_set_expiry(const std::string &key, uint db, db::time_t expiry_time);

	// log a transaction that's about to happen
	void wal_transaction(uint db, const std::string &commands, size_t size);

	// obtain a reference to the mutex
	std::mutex &wal_mutex();


	/*
	 * RAII mechanism to close and reopen a WriteAheadLogger
	 */
	class Closed {
	private:
		// the WriteAheadLogger
		WriteAheadLogger &m_wal;

		// the WAL file to reopen with
		std::filesystem::path m_wal_file;

	public:
		// close the Logger
		Closed(WriteAheadLogger &wal, std::filesystem::path wal_file)
			: m_wal{wal}, m_wal_file{std::move(wal_file)} {
			m_wal.close_wal();
		}

		// reopen the Logger
		~Closed() {
			m_wal.wal_to(m_wal_file);
		}
	};
};

}

#endif //VANITY_WRITE_AHEAD_LOGGER_H

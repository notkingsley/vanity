//
// Created by kingsli on 8/7/24.
//

#ifndef VANITY_WRITE_AHEAD_LOGGER_H
#define VANITY_WRITE_AHEAD_LOGGER_H

#include <filesystem>
#include <fstream>
#include <mutex>

#include "db/db/db_operations.h"
#include "db/trn_constants.h"
#include "utils/serializer.h"
#include "wal_entry_t.h"


namespace vanity::wal {

/*
 * A WriteAheadLogger logs the operations performed on a database
 */
class WriteAheadLogger
{
private:
	template<std::size_t n, typename T, typename... Tp>
	struct is_nth_type;

	template<size_t n, typename T, typename Head, typename... Tail>
	struct is_nth_type<n, T, Head, Tail...>
		: is_nth_type<n - 1, T, Tail...> { };

	template<typename T, typename Head, typename... Tail>
	struct is_nth_type<0, T, Head, Tail...>
		: std::conditional<std::is_same_v<T, Head>, std::true_type, std::false_type>::type { };

	template<size_t n, typename T>
	struct is_nth_type<n, T> : std::false_type { };

	// true if the nth type in Args is T
	template<size_t n, typename T, typename... Args>
	static constexpr bool is_nth_type_v = is_nth_type<n, T, Args...>::value;

	// validate parameter types on Args
	template<typename ...Args>
	static constexpr void validate_types(const Args &...) {
		constexpr bool is_wal_entry_0 = is_nth_type_v<0, wal_entry_t, Args...>;
		constexpr bool is_uint_1 = is_nth_type_v<1, uint, Args...>;
		constexpr bool is_string_2 = is_nth_type_v<2, std::string, Args...>;
		constexpr bool is_trn_id_2 = is_nth_type_v<2, trn_id_t, Args...>;
		constexpr bool is_db_op_3 = is_nth_type_v<3, db::db_op_t, Args...>;

		static_assert(is_wal_entry_0, "First argument must be of type wal_entry_t");
		static_assert(is_uint_1, "Second argument must be of type uint");
		static_assert((is_string_2) or (is_trn_id_2 and is_db_op_3),
			"Third argument must be of type std::string or trn_id_t and fourth argument must be of type db::db_op_t"
		);
	}

	// the WAL file
	std::ofstream m_wal_file;

	// mutex for the WAL
	std::mutex m_wal_mutex;


	// write an entry to the wal
	template<typename ...Args>
	void wal(const Args &... args) {
		validate_types(args...);

		std::lock_guard lock(m_wal_mutex);
		if (not m_wal_file.is_open())
			return;

		serializer::WriteHandle writer{m_wal_file};
		writer.write(args...);
		std::endl(m_wal_file);
	}

	// close the WAL
	void close_wal();

public:
	// use file for the WAL
	void wal_to(const std::filesystem::path &wal_file);

	// log a db operation that's about to happen
	template<typename ...Args>
	void wal_db_op(uint db, trn_id_t trn_id, db::db_op_t op, const Args &... args) {
		wal(wal_entry_t::db_op, db, trn_id, op, args...);
	}

	// log an expiry that's about to happen
	void wal_expiry(const std::string &key, uint db);


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

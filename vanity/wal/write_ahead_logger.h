//
// Created by kingsli on 8/7/24.
//

#ifndef VANITY_WRITE_AHEAD_LOGGER_H
#define VANITY_WRITE_AHEAD_LOGGER_H

#include <filesystem>
#include <fstream>
#include <mutex>

#include "client/client.h"
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

	// write an entry to the wal
	template<typename ...Args>
	void wal(const Args &... args);

public:
	// use file for the WAL
	void wal_to(const std::filesystem::path &wal_file);

	// close the WAL
	void close_wal();

	// log a request that's about to happen
	// requires op to be the operation extracted from the request
	void wal_request(uint db, const std::string_view &request);

	// log an expiry that's about to happen
	void wal_expiry(const std::string &key, uint db);

	// log a set_expiry operation that's about to happen
	// this is WAL-ed separately because the replay of SET_EXPIRY
	// with a WAL is dependent on a time factor,
	// violating the invariance on the principle of
	// WAL: we must arrive at exactly the same state after
	// redoing each entry in the WAL
	void wal_set_expiry(const std::string& key, uint db, db::time_t expiry_time);

	// log a transaction that's about to happen
	void wal_transaction(uint db, const std::string& commands, size_t size);

	// obtain a reference to the mutex
	std::mutex &wal_mutex();
};

}

#endif //VANITY_WRITE_AHEAD_LOGGER_H

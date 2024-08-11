//
// Created by kingsli on 8/7/24.
//

#include "write_ahead_logger.h"

namespace vanity::wal {

void WriteAheadLogger::wal_to(const std::filesystem::path &wal_file) {
	std::lock_guard lock(m_wal_mutex);
	if (m_wal_file.is_open())
		m_wal_file.close();
	m_wal_file.open(wal_file, std::ios::out | std::ios::app);
}

void WriteAheadLogger::close_wal() {
	std::lock_guard lock(m_wal_mutex);
	if (m_wal_file.is_open())
		m_wal_file.close();
}

void WriteAheadLogger::wal_request(uint db, const std::string_view &request) {
	// TODO: remove wal_request
	// wal(wal_entry_t::request, db, request);
}

void WriteAheadLogger::wal_expiry(const std::string &key, uint db) {
	wal(wal_entry_t::expire, db, key);
}

void WriteAheadLogger::wal_set_expiry(const std::string &key, uint db, db::time_t expiry_time) {
	wal(wal_entry_t::set_expiry, db, key, expiry_time);
}

void WriteAheadLogger::wal_transaction(uint db, const std::string &commands, size_t size) {
	wal(wal_entry_t::transaction, db, commands, size);
}

std::mutex &WriteAheadLogger::wal_mutex() {
	return m_wal_mutex;
}

} // namespace vanity::wal

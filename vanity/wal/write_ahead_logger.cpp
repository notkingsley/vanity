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

void WriteAheadLogger::wal_expiry(const std::string &key, uint db) {
	wal(wal_entry_t::expire, db, key);
}

void WriteAheadLogger::wal_transaction(uint db, const std::string &commands, size_t size) {
	wal(wal_entry_t::transaction, db, commands, size);
}

std::mutex &WriteAheadLogger::wal_mutex() {
	return m_wal_mutex;
}

} // namespace vanity::wal

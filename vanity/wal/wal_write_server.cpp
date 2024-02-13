//
// Created by kingsli on 2/12/24.
//

#include "wal_write_server.h"

namespace vanity {

void WALWriteServer::wal_to(const std::filesystem::path &wal_file) {
	std::lock_guard lock(m_wal_mutex);
	if (m_wal_file.is_open())
		m_wal_file.close();
	m_wal_file.open(wal_file, std::ios::out | std::ios::app);
}

void WALWriteServer::close_wal() {
	std::lock_guard lock(m_wal_mutex);
	if (m_wal_file.is_open())
		m_wal_file.close();
}

void WALWriteServer::write_ahead(Client &client, operation_t op, const std::string_view &request) {
	if (not should_wal(op))
		return;

	std::lock_guard lock(m_wal_mutex);
	if (not m_wal_file.is_open())
		return;

	serializer::write(m_wal_file, session_db(client));
	serializer::write(m_wal_file, request);
	m_wal_file << std::endl;
}

} // namespace vanity
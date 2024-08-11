//
// Created by kingsli on 2/12/24.
//

#include "wal_write_server.h"

namespace vanity::wal {

void WalWriteServer::wal_to(const std::filesystem::path &wal_file) {
	m_logger.wal_to(wal_file);
}

void WalWriteServer::wal_transaction(Client &client, const std::string &commands, size_t size) {
	m_logger.wal_transaction(session_db(client), commands, size);
}

std::mutex &WalWriteServer::wal_mutex() {
	return m_logger.wal_mutex();
}

} // namespace vanity::wal

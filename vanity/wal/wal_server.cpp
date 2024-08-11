//
// Created by kingsli on 2/9/24.
//

#include "wal_server.h"

namespace vanity::wal {

WalServer::WalServer() {
	if (m_wal_file)
		wal_to(*m_wal_file);
}

void WalServer::recover() {
	if (not m_wal_file)
		return;

	auto &file = *m_wal_file;
	if (not exists(file))
		return;

	enable_databases_expiry(false);
	{
		WriteAheadLogger::Closed closed {wal_logger(), file};
		recover_from(file);
	}

	enable_databases_expiry(true);
	deep_purge_databases();
}

} // namespace vanity::wal
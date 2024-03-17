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

	pre_recovery();
	{
		ClosedWal closed {*this, file};
		recover_from(file);
	}
	post_recovery();
}

} // namespace vanity::wal
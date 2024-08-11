//
// Created by kingsli on 3/15/24.
//

#include "wal_expiry_server.h"

namespace vanity::wal {

void WalExpiryServer::pre_recovery() {
	for (auto& db: m_databases)
		db.expiry_enabled(false);
}

void WalExpiryServer::post_recovery() {
	for (auto& db: m_databases)
		db.expiry_enabled(true);

	deep_purge_databases();
}

void WalExpiryServer::pre_persist() {
	deep_purge_databases();
}

} // namespace vanity::wal
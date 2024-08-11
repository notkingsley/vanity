//
// Created by kingsli on 3/15/24.
//

#include "wal_expiry_server.h"

namespace vanity::wal {

void WalExpiryServer::deep_purge_all() {
	for (auto& db: m_databases)
		db.deep_purge();
}

void WalExpiryServer::pre_recovery() {
	for (auto& db: m_databases)
		db.expiry_enabled(false);
}

void WalExpiryServer::post_recovery() {
	for (auto& db: m_databases)
		db.expiry_enabled(true);

	deep_purge_all();
}

void WalExpiryServer::pre_persist() {
	deep_purge_all();
}

void WalExpiryServer::wal_redo_expire(const std::string &key, uint db) {
	database(db).force_expire(key);
}

} // namespace vanity::wal
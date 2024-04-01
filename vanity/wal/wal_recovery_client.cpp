//
// Created by kingsli on 2/12/24.
//

#include "wal_recovery_client.h"

namespace vanity::wal {

bool WalRecoveryClient::has_perm(operation_t op) const {
	return true;
}

struct session_info &WalRecoveryClient::session_info() {
	return m_session_info;
}

void WalRecoveryClient::close() {
	throw std::runtime_error("WalRecoveryClient::close() called");
}

void WalRecoveryClient::write(WriteManager& manager, Response &&response) { }

} // namespace vanity::wal
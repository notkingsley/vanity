//
// Created by kingsli on 2/12/24.
//

#include "wal_recovery_client.h"

namespace vanity {


void WalRecoveryClient::ready(SocketServer &server) {
	throw std::runtime_error("WalRecoveryClient::ready() called");
}

bool WalRecoveryClient::has_perm(operation_t op) const {
	return true;
}

struct session_info &WalRecoveryClient::session_info() {
	return m_session_info;
}

void WalRecoveryClient::close() {
	throw std::runtime_error("WalRecoveryClient::close() called");
}

size_t WalRecoveryClient::read(char *buffer, size_t buffer_size) const {
	throw std::runtime_error("WalRecoveryClient::read() called");
}

void WalRecoveryClient::write(SocketServer &server, Response &&response) { }

} // namespace vanity
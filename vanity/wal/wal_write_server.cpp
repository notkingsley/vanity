//
// Created by kingsli on 2/12/24.
//

#include "wal_write_server.h"

namespace vanity {

// return true if the client is currently
// executing a transaction
// defined in transaction_server.cpp
extern bool is_executing_transaction(Client& client);

namespace wal {

// true if the nth type in Args is
template<int n, typename T, typename... Args>
constexpr bool is_of_type = std::is_same_v<T, std::tuple_element_t<n, std::tuple<Args...>>>;

void WalWriteServer::wal_to(const std::filesystem::path &wal_file) {
	m_logger.wal_to(wal_file);
}

void WalWriteServer::close_wal() {
	m_logger.close_wal();
}

void WalWriteServer::wal_request(Client &client, operation_t op, const std::string_view &request) {
	if (not should_wal(op))
		return;

	if (is_executing_transaction(client))
		return;

	m_logger.wal_request(session_db(client), request);
}

void WalWriteServer::wal_expiry(const std::string &key, uint db) {
	m_logger.wal_expiry(key, db);
}

void WalWriteServer::wal_set_expiry(const std::string &key, uint db, db::time_t expiry_time) {
	m_logger.wal_set_expiry(key, db, expiry_time);
}

void WalWriteServer::wal_transaction(Client &client, const std::string &commands, size_t size) {
	m_logger.wal_transaction(session_db(client), commands, size);
}

std::mutex &WalWriteServer::wal_mutex() {
	return m_logger.wal_mutex();
}

ClosedWal::ClosedWal(WalWriteServer &wal, std::filesystem::path wal_file)
	: m_wal{wal}, m_wal_file{std::move(wal_file)} {
	m_wal.close_wal();
}

ClosedWal::~ClosedWal() {
	m_wal.wal_to(m_wal_file);
}

} // namespace wal

} // namespace vanity
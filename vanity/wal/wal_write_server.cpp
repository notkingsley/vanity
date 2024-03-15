//
// Created by kingsli on 2/12/24.
//

#include "wal_write_server.h"

namespace vanity::wal {

void WalWriteServer::wal_to(const std::filesystem::path &wal_file) {
	std::lock_guard lock(m_wal_mutex);
	if (m_wal_file.is_open())
		m_wal_file.close();
	m_wal_file.open(wal_file, std::ios::out | std::ios::app);
}

void WalWriteServer::close_wal() {
	std::lock_guard lock(m_wal_mutex);
	if (m_wal_file.is_open())
		m_wal_file.close();
}

void WalWriteServer::wal_request(Client &client, operation_t op, const std::string_view &request) {
	if (not should_wal(op))
		return;

	std::lock_guard lock(m_wal_mutex);
	if (not m_wal_file.is_open())
		return;

	serializer::write(m_wal_file, wal_entry_t::request);
	serializer::write(m_wal_file, session_db(client));
	serializer::write(m_wal_file, request);
	m_wal_file << std::endl;
}

void WalWriteServer::wal_expiry(const std::string &key) {
	std::lock_guard lock(m_wal_mutex);
	if (not m_wal_file.is_open())
		return;

	serializer::write(m_wal_file, wal_entry_t::expire);
	serializer::write(m_wal_file, key);
	m_wal_file << std::endl;
}

std::mutex &WalWriteServer::wal_mutex() {
	return m_wal_mutex;
}

ClosedWAL::ClosedWAL(WalWriteServer &wal, std::filesystem::path wal_file)
	: m_wal{wal}, m_wal_file{std::move(wal_file)}
{
	m_wal.close_wal();
}

ClosedWAL::~ClosedWAL() {
	m_wal.wal_to(m_wal_file);
}

} // namespace vanity::wal
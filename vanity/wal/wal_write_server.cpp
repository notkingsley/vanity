//
// Created by kingsli on 2/12/24.
//

#include "wal_write_server.h"

namespace vanity::wal {

// true if the nth type in Args is T
template<int n, typename T, typename... Args>
constexpr bool is_of_type = std::is_same_v<T, std::tuple_element_t<n, std::tuple<Args...>>>;

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

template<typename... Args>
void WalWriteServer::wal(const Args&... args) {
	static_assert(is_of_type<0, wal_entry_t, Args...>, "First argument must be of type wal_entry_t");
	static_assert(is_of_type<1, uint, Args...>, "Second argument must be of type uint");
	static_assert(
		is_of_type<2, std::string, Args...> || is_of_type<2, std::string_view, Args...>,
		"Third argument must be of type std::string or std::string_view"
	);

	std::lock_guard lock(m_wal_mutex);
	if (not m_wal_file.is_open())
		return;

	(serializer::write(m_wal_file, args), ...);
	m_wal_file << std::endl;
}

void WalWriteServer::wal_request(Client &client, operation_t op, const std::string_view &request) {
	if (should_wal(op))
		wal(wal_entry_t::request, session_db(client), request);
}

void WalWriteServer::wal_expiry(const std::string &key, uint db) {
	wal(wal_entry_t::expire, db, key);
}

void WalWriteServer::wal_set_expiry(const std::string &key, uint db, db::time_t expiry_time) {
	wal(wal_entry_t::set_expiry, db, key, expiry_time);
}

std::mutex &WalWriteServer::wal_mutex() {
	return m_wal_mutex;
}

ClosedWal::ClosedWal(WalWriteServer &wal, std::filesystem::path wal_file)
	: m_wal{wal}, m_wal_file{std::move(wal_file)}
{
	m_wal.close_wal();
}

ClosedWal::~ClosedWal() {
	m_wal.wal_to(m_wal_file);
}

} // namespace vanity::wal
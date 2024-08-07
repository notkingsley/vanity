//
// Created by kingsli on 8/7/24.
//

#include "utils/serializer.h"
#include "write_ahead_logger.h"

namespace vanity {

// return true if the client is currently
// executing a transaction
// defined in transaction_server.cpp
extern bool is_executing_transaction(Client& client);

namespace wal {

// true if the nth type in Args is
template<int n, typename T, typename... Args>
constexpr bool is_of_type = std::is_same_v<T, std::tuple_element_t<n, std::tuple<Args...>>>;

void WriteAheadLogger::wal_to(const std::filesystem::path &wal_file) {
	std::lock_guard lock(m_wal_mutex);
	if (m_wal_file.is_open())
		m_wal_file.close();
	m_wal_file.open(wal_file, std::ios::out | std::ios::app);
}

void WriteAheadLogger::close_wal() {
	std::lock_guard lock(m_wal_mutex);
	if (m_wal_file.is_open())
		m_wal_file.close();
}

template<typename... Args>
void WriteAheadLogger::wal(const Args&... args) {
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

void WriteAheadLogger::wal_request(uint db, const std::string_view &request) {
	wal(wal_entry_t::request, db, request);
}

void WriteAheadLogger::wal_expiry(const std::string &key, uint db) {
	wal(wal_entry_t::expire, db, key);
}

void WriteAheadLogger::wal_set_expiry(const std::string &key, uint db, db::time_t expiry_time) {
	wal(wal_entry_t::set_expiry, db, key, expiry_time);
}

void WriteAheadLogger::wal_transaction(uint db, const std::string &commands, size_t size) {
	wal(wal_entry_t::transaction, db, commands, size);
}

std::mutex &WriteAheadLogger::wal_mutex() {
	return m_wal_mutex;
}

} // namespace wal

} // namespace vanity
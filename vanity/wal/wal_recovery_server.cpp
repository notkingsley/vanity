//
// Created by kingsli on 2/12/24.
//

#include "wal_entry_t.h"
#include "wal_recovery_client.h"
#include "wal_recovery_server.h"


namespace vanity::wal {

void WalRecoveryServer::recover_from(const std::filesystem::path &wal_file)
{
	std::array<WalRecoveryClient, M_NUM_DATABASES> clients;
	for (uint i = 0; i < M_NUM_DATABASES; ++i)
		session_db(clients[i]) = i;

	auto eof = std::ifstream::traits_type::eof;
	std::ifstream wal{wal_file};
	while (wal.peek() != eof())
	{
		auto entry_t = serializer::read<wal_entry_t>(wal);
		auto db = serializer::read<uint>(wal);
		auto body = serializer::read<std::string>(wal);

		switch (entry_t) {
			case wal_entry_t::request:
			{
				wal_redo_request(body, clients[db]);
				break;
			}
			case wal_entry_t::expire:
			{
				wal_redo_expire(body, db);
				break;
			}
			case wal_entry_t::set_expiry:
			{
				auto expiry_time = serializer::read<db::time_t>(wal);
				wal_redo_set_expiry(body, db, expiry_time);
				break;
			}
			case wal_entry_t::transaction:
			{
				auto size = serializer::read<size_t>(wal);
				wal_redo_transaction(clients[db], body, size);
				break;
			}
			default:
			{
				throw WALError("Bad wal_entry_t");
			}
		}

		if (wal.get() != '\n')
			throw WALError("WAL file is corrupted: no newline after entry");
	}
}

} // namespace vanity::wal
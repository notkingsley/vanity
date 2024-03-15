//
// Created by kingsli on 2/12/24.
//

#include "wal_entry_t.h"
#include "wal_recovery_server.h"


namespace vanity::wal {

void WalRecoveryServer::recover_from(const std::filesystem::path &wal_file)
{
	std::array<WalRecoveryClient, M_NUM_DATABASES> clients;
	for (int i = 0; i < M_NUM_DATABASES; ++i)
		session_db(clients[i]) = i;

	std::ifstream wal{wal_file};
	while (wal.peek() != std::ifstream::traits_type::eof())
	{
		auto entry_t = serializer::read<wal_entry_t>(wal);
		auto db = serializer::read<uint>(wal);
		auto body = serializer::read<std::string>(wal);

		switch (entry_t) {
			case wal_entry_t::request: {
				handle(body, clients[db]);
				break;
			}
			case wal_entry_t::expire: {
				m_databases[db].force_expire(body);
				break;
			}
			default:
				throw std::runtime_error("Bad wal_entry_t");
		}

		if (wal.get() != '\n')
			throw std::runtime_error("WAL file is corrupted: no newline after entry");
	}
}

} // namespace vanity::wal
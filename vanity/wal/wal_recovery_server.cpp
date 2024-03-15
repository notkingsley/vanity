//
// Created by kingsli on 2/12/24.
//

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
		auto db = serializer::read<int>(wal);
		auto request = serializer::read<std::string>(wal);
		handle(request, clients[db]);

		if (wal.get() != '\n')
			throw std::runtime_error("WAL file is corrupted: no newline after entry");
	}
}

} // namespace vanity::wal
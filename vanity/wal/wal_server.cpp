//
// Created by kingsli on 2/9/24.
//

#include "wal_server.h"

namespace vanity {

WALServer::WALServer(const std::optional<std::filesystem::path> &wal_file)
	:m_wal_file(wal_file)
{
	if (m_wal_file)
		wal_to(*m_wal_file);
}

void WALServer::recover() {
	if (not m_wal_file)
		return;

	auto &file = *m_wal_file;
	if (std::filesystem::exists(file)) {
		close_wal();
		recover_from(file);
		wal_to(file);
	}
}

} // namespace vanity
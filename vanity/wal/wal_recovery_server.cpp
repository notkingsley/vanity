//
// Created by kingsli on 2/12/24.
//

#include "wal_entry_t.h"
#include "wal_recovery_client.h"
#include "wal_recovery_server.h"


namespace vanity::wal {

void WalRecoveryServer::do_recover()
{
	std::array<WalRecoveryClient, M_NUM_DATABASES> clients;
	for (uint i = 0; i < M_NUM_DATABASES; ++i) {
		session_set_auth(clients[i], client_auth::USER);
		session_db(clients[i]) = i;
	}

	auto get_db = [this](uint db) -> auto& { return database(db); };
	auto eof = std::ifstream::traits_type::eof();
	std::ifstream wal{*m_wal_file};

	while (wal.peek() != eof)
	{
		auto entry_t = serializer::read<wal_entry_t>(wal);
		auto db = serializer::read<uint>(wal);

		switch (entry_t) {
			case wal_entry_t::db_op:
			{
				auto op = serializer::read<db::db_op_t>(wal);
				database(db).wal_redo_db_op(op, wal, get_db);
				break;
			}
			case wal_entry_t::expire:
			{
				auto body = serializer::read<std::string>(wal);
				database(db).wal_redo_expiry(body);
				break;
			}
			case wal_entry_t::transaction:
			{
				auto body = serializer::read<std::string>(wal);
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

WalRecoveryServer::WalRecoveryServer() {
	if (m_wal_file)
		wal_to(*m_wal_file);
}

void WalRecoveryServer::recover() {
	if (not m_wal_file)
		return;

	auto &file = *m_wal_file;
	if (not exists(file))
		return;

	enable_databases_expiry(false);
	{
		WriteAheadLogger::Closed closed {wal_logger(), file};
		do_recover();
	}

	enable_databases_expiry(true);
	deep_purge_databases();
}

} // namespace vanity::wal
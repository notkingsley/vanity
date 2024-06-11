//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_TRANSACTION_SERVER_H
#define VANITY_TRANSACTION_SERVER_H

#include "db/servers/base_database_server.h"
#include "client/write_manager.h"

namespace vanity {

/*
 * A TransactionServer handles transaction requests
 */
class TransactionServer:
	public virtual BaseDatabaseServer,
	public virtual WriteManager
{
public:
	// a transact_begin request was received from a client
	void request_transact_begin(Client& client) override;

	// a transact_commit request was received from a client
	void request_transact_commit(Client& client) override;

	// a transact_discard request was received from a client
	void request_transact_discard(Client& client) override;

	// dispatch a request in a transaction context
	bool dispatch_transaction_context(Client& client, Request& request, bool end) override;

	// redo a previously WAL-ed transaction
	void wal_redo_transaction(Client& client, const std::string& commands, size_t size);

private:
	// get a reference to a client's current transaction data
	static user_data_t::transaction_data_t& data(Client& client);

	// begin_transaction transaction for a client
	static void begin_transaction(Client& client);

	// exit transaction for a client
	static void exit_transaction(Client& client);
};

} // namespace vanity

#endif //VANITY_TRANSACTION_SERVER_H

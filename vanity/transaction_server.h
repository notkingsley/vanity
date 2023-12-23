//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_TRANSACTION_SERVER_H
#define VANITY_TRANSACTION_SERVER_H

#include "db/base_database_server.h"
#include "request_server.h"
#include "socket/socket_server.h"

namespace vanity {

/*
 * A TransactionServer handles transaction requests
 */
class TransactionServer:
	public virtual BaseDatabaseServer,
	public virtual RequestServer,
	public virtual SocketServer
{
public:
	// a transact_begin request was received from a client
	void request_transact_begin(Client& client) override;

	// a transact_commit request was received from a client
	void request_transact_commit(Client& client) override;

	// a transact_discard request was received from a client
	void request_transact_discard(Client& client) override;

	// dispatch a request in a transaction context
	bool dispatch_transaction_request(Client& client, Request& request, bool end, bool strict) override;

private:
	// push a validated request to the transaction
	static void push(Client& client, std::string_view request);

	// get a reference to a client's current transaction data
	static transaction_data& data(Client& client);

	// begin_transaction transaction for a client
	static void begin_transaction(Client& client);

	// exit transaction for a client
	static void exit_transaction(Client& client);
};

} // namespace vanity

#endif //VANITY_TRANSACTION_SERVER_H

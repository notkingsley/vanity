//
// Created by kingsli on 9/2/24.
//

#ifndef VANITY_TRANSACTION_SERVER_H
#define VANITY_TRANSACTION_SERVER_H

#include "abstract_server.h"
#include "atomic_counter.h"
#include "db/servers/database_object_server.h"
#include "request/request_handler.h"
#include "response/response_server.h"


namespace vanity {

/*
 * A TransactionServer allows a transaction to be
 * repeatedly pushed after some constant time interval
 */
class TransactionServer:
	public virtual AbstractServer,
	public virtual DatabaseObjectServer,
	public virtual RequestHandler,
	public virtual ResponseServer
{
private:
	// default transaction id
	static constexpr uint64_t DEFAULT_TRN_ID = 0;

	// the transaction id counter
	AtomicCounter m_counter;

	// get a new transaction id
	uint64_t new_trn_id();

public:
	// a transact_begin request was received from a client
	void request_transact_begin(Client& client) override;

	// a transact_commit request was received from a client
	void request_transact_commit(Client& client) override;

	// a transact_discard request was received from a client
	void request_transact_discard(Client& client) override;
};

} // namespace vanity

#endif //VANITY_TRANSACTION_SERVER_H

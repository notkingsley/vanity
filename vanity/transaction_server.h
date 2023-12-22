//
// Created by kingsli on 12/21/23.
//

#ifndef VANITY_TRANSACTION_SERVER_H
#define VANITY_TRANSACTION_SERVER_H

#include "request_server.h"

namespace vanity {

/*
 * A TransactionServer handles transaction requests
 */
class TransactionServer : public virtual RequestServer
{
public:
	// a transact_begin request was received from a client
	void request_transact_begin(Client& client) override;

	// a transact_commit request was received from a client
	void request_transact_commit(Client& client) override;

	// a transact_discard request was received from a client
	void request_transact_discard(Client& client) override;

	// dispatch a request in a transaction context
	virtual bool dispatch_transaction_request(Client& client, Request& request, bool end, bool strict);
};

} // namespace vanity

#endif //VANITY_TRANSACTION_SERVER_H

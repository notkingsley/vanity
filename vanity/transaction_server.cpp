//
// Created by kingsli on 12/21/23.
//

#include "transaction_server.h"

namespace vanity {

void TransactionServer::request_transact_begin(Client &client) { }

void TransactionServer::request_transact_commit(Client &client) { }

void TransactionServer::request_transact_discard(Client &client) { }

bool TransactionServer::dispatch_transaction_request(Client &client, Request &request, bool end, bool strict) { return true; }

} // namespace vanity
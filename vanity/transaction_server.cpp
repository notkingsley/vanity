//
// Created by kingsli on 12/21/23.
//

#include "client/aggregating_client.h"
#include "request/request_tracker.h"
#include "transaction_server.h"


namespace vanity {

/*
 * A TransactionClient is an AggregatingClient for transaction requests
 */
class TransactionClient : public AggregatingClient
{
public:
	// create a TransactionClient with an underlying client and a response size
	TransactionClient(Client& client, size_t size)
		: AggregatingClient(client, size) {};
};

// return true if the client is currently
// executing a transaction
bool is_executing_transaction(Client& client) {
	return dynamic_cast<TransactionClient*>(&client);
}

void TransactionServer::request_transact_begin(Client &client) {
	begin_transaction(client);
	send(client, ok());
}

void TransactionServer::request_transact_commit(Client &client) {
	auto& trn_data {data(client)};
	TransactionClient trn_client {client, trn_data.size};
	Request trn_request {trn_data.commands};

	{
		auto trn_lock {database(client).lock()};
		wal_transaction(client, trn_data.commands, trn_data.size);
		do_handle(trn_client, trn_request, trn_data.size);
	}

	exit_transaction(client);
	trn_client.perform_write(*this);
}

void TransactionServer::request_transact_discard(Client &client) {
	exit_transaction(client);
	send(client, ok());
}

bool TransactionServer::dispatch_transaction_context(Client &client, Request &request, bool end) {
	if (is_executing_transaction(client))
		return dispatch_normal_context(client, request, end);

	RequestTracker tracker {request};
	dry_dispatch(request, end);
	data(client).push(tracker.view());
	send(client, queued());
	return true;
}

void TransactionServer::wal_redo_transaction(Client &client, const std::string &commands, size_t size) {
	begin_transaction(client);
	data(client) = {commands, size};
	request_transact_commit(client);
}

transaction_data& TransactionServer::data(Client &client) {
	return session_data<conn_state::TRANSACTION>(client);
}

void TransactionServer::begin_transaction(Client &client) {
	session_set_state(client, conn_state::TRANSACTION);
}

void TransactionServer::exit_transaction(Client &client) {
	session_set_state(client, conn_state::NORMAL);
}

} // namespace vanity
//
// Created by kingsli on 12/21/23.
//

#include "client/aggregating_client.h"
#include "request_tracker.h"
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
		: AggregatingClient(client, size, "TRANSACTION") {};
};

void TransactionServer::request_transact_begin(Client &client) {
	begin_transaction(client);
	send(client, ok());
}

void TransactionServer::request_transact_commit(Client &client) {
	auto &trn_data = data(client);
	TransactionClient trn_client{client, trn_data.size};
	Request trn_request{trn_data.commands};

	{
		TempState trn_state{client, conn_state::NORMAL};
		auto trn_lock= database(client).lock();

		for (size_t i = 0; i < trn_data.size - 1; ++i)
			do_handle(trn_client, trn_request, false, true);
		do_handle(trn_client, trn_request, true, true);
	}

	exit_transaction(client);
	trn_client.perform_write(*this);
}

void TransactionServer::request_transact_discard(Client &client) {
	exit_transaction(client);
	send(client, ok());
}

bool TransactionServer::dispatch_transaction_request(Client &client, Request &request, bool end, bool strict) {
	RequestTracker tracker{request};
	operation_t op = request.get_operation();
	dry_dispatch_op(op, request, end);
	push(client, tracker.view());
	send(client, queued());
	return true;
}

void TransactionServer::push(Client &client, std::string_view request) {
	data(client).push(request);
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
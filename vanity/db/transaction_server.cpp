//
// Created by kingsli on 9/2/24.
//

#include "transaction_server.h"

namespace vanity {

uint64_t TransactionServer::new_trn_id() {
	return m_counter.next();
}

void TransactionServer::request_transact_begin(Client &client) {
	auto& trn_id {session_trn_id(client)};
	if (trn_id != NO_TRN_ID)
		// TODO: replace with a StopRequest
		throw std::runtime_error("Transaction already in progress");

	trn_id = database_obj(client).begin(new_trn_id());
	send(client, ok());
}

void TransactionServer::request_transact_commit(Client &client) {
	auto& trn_id {session_trn_id(client)};
	if (trn_id == NO_TRN_ID)
		// TODO: replace with a StopRequest
		throw std::runtime_error("No transaction in progress");

	database_obj(client).commit(trn_id);
	trn_id = NO_TRN_ID;

	send(client, ok());
}

void TransactionServer::request_transact_discard(Client &client) {
	auto& trn_id {session_trn_id(client)};
	if (trn_id == NO_TRN_ID)
		// TODO: replace with a StopRequest
		throw std::runtime_error("No transaction in progress");

	database_obj(client).discard(trn_id);
	trn_id = NO_TRN_ID;

	send(client, ok());
}

} // namespace vanity

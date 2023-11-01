//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_LIST_DATABASE_SERVER_H
#define VANITY_LIST_DATABASE_SERVER_H

#include "base_database_server.h"

namespace vanity {

/*
 * A ListDatabaseServer implements the functionalities responsible for
 * the database's list type
 */
class ListDatabaseServer : public virtual BaseDatabaseServer
{
public:
	// a list_len request was received from a client
	void request_list_len(Client& client, const std::string& key) override;

	// a list_get request was received from a client
	void request_list_get(Client& client, const std::string& key, int64_t index) override;

	// a list_set request was received from a client
	void request_list_set(Client& client, const std::string& key, int64_t index, const std::string& value) override;

	// a list_push_left request was received from a client
	void request_list_push_left(Client& client, const std::string& key, std::list<std::string> values) override;

	// a list_push_right request was received from a client
	void request_list_push_right(Client& client, const std::string& key, std::list<std::string> values) override;

	// a list_pop_left request was received from a client
	void request_list_pop_left(Client& client, const std::string& key, int64_t count) override;

	// a list_pop_right request was received from a client
	void request_list_pop_right(Client& client, const std::string& key, int64_t count) override;

	// a list_range request was received from a client
	void request_list_range(Client& client, const std::string& key, int64_t start, int64_t stop) override;

	// a list_trim request was received from a client
	void request_list_trim(Client& client, const std::string& key, int64_t start, int64_t stop) override;

	// a list_remove request was received from a client
	void request_list_remove(Client& client, const std::string& key, const std::string& value, int64_t count) override;

private:
	// handle a variant result returned by a list request
	template<class T>
	void handle_result(Client& client, const std::variant<T, db::ListErrorKind>& result);

	// send a db::ListErrorKind to the client
	void send_error(Client& client, db::ListErrorKind kind);
};

} // namespace vanity

#endif //VANITY_LIST_DATABASE_SERVER_H

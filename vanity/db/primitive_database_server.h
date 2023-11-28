//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_PRIMITIVE_DATABASE_SERVER_H
#define VANITY_PRIMITIVE_DATABASE_SERVER_H

#include "base_database_server.h"

namespace vanity {

/*
 * A PrimitiveDatabaseServer implements the functionalities responsible for
 * the database's primitive types (int, float, string)
 */
class PrimitiveDatabaseServer : public virtual BaseDatabaseServer
{
public:
	// a set_str request was received from a client
	virtual void request_set_str(Client& client, const std::string& key, const std::string& value) override;

	// a set_int request was received from a client
	virtual void request_set_int(Client& client, const std::string& key, const int64_t& value) override;

	// a set_int request was received from a client
	virtual void request_set_float(Client& client, const std::string& key, const double& value) override;

	// an incr_int request was received from a client
	void request_incr_int(Client& client, const std::string& key, db::int_t value) override;

	// an incr_float request was received from a client
	void request_incr_float(Client& client, const std::string& key, db::float_t value) override;

	// a str_len request was received from a client
	void request_str_len(Client& client, const std::string& key) override;

	// a many_get request was received from a client
	void request_many_get(Client& client, const std::vector<std::string>& keys) override;
};

} // namespace vanity

#endif //VANITY_PRIMITIVE_DATABASE_SERVER_H

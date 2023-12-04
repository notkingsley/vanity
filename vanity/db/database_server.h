//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_DATABASE_SERVER_H
#define VANITY_DATABASE_SERVER_H

#include "auto_expiry_database_server.h"
#include "expiry_database_server.h"
#include "hash_database_server.h"
#include "list_database_server.h"
#include "persistent_server.h"
#include "primitive_database_server.h"
#include "set_database_server.h"

namespace vanity {

/*
 * A DatabaseServer allows us to respond to database requests
 */
class DatabaseServer :
	public virtual AutoExpiryDatabaseServer,
	public virtual ExpiryDatabaseServer,
	public virtual HashDatabaseServer,
	public virtual ListDatabaseServer,
	public virtual PrimitiveDatabaseServer,
	public virtual PersistentServer,
	public virtual SetDatabaseServer
{
public:
	// start any background tasks
	void start(){
		BaseDatabaseServer::start();
		PersistentServer::start();
		AutoExpiryDatabaseServer::start();
	}

	// stop any background tasks
	void stop(){
		AutoExpiryDatabaseServer::stop();
		PersistentServer::stop();
		BaseDatabaseServer::stop();
	}
};

} // namespace vanity

#endif //VANITY_DATABASE_SERVER_H

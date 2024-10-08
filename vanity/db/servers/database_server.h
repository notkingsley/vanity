//
// Created by kingsli on 9/14/23.
//

#ifndef VANITY_DATABASE_SERVER_H
#define VANITY_DATABASE_SERVER_H

#include "expiry_database_server.h"
#include "general_database_server.h"
#include "hash_database_server.h"
#include "list_database_server.h"
#include "primitive_database_server.h"
#include "set_database_server.h"

namespace vanity {

/*
 * A DatabaseServer allows us to respond to database requests
 */
class DatabaseServer :
	public virtual ExpiryDatabaseServer,
	public virtual GeneralDatabaseServer,
	public virtual HashDatabaseServer,
	public virtual ListDatabaseServer,
	public virtual PrimitiveDatabaseServer,
	public virtual SetDatabaseServer { };

} // namespace vanity

#endif //VANITY_DATABASE_SERVER_H

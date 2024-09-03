//
// Created by kingsli on 10/29/23.
//

#ifndef VANITY_BASE_DATABASE_SERVER_H
#define VANITY_BASE_DATABASE_SERVER_H

#include "abstract_server.h"
#include "database_object_server.h"
#include "log_server.h"
#include "request/request_handler.h"
#include "response/response_server.h"


namespace vanity {

/*
 * A BaseDatabaseServer holds the common functionality of a DatabaseServer
 */
class BaseDatabaseServer:
	public virtual AbstractServer,
	public virtual DatabaseObjectServer,
	public virtual LogServer,
	public virtual RequestHandler,
	public virtual ResponseServer { };

} // namespace vanity

#endif //VANITY_BASE_DATABASE_SERVER_H

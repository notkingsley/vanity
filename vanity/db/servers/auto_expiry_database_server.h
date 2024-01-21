//
// Created by kingsli on 12/4/23.
//

#ifndef VANITY_AUTO_EXPIRY_DATABASE_SERVER_H
#define VANITY_AUTO_EXPIRY_DATABASE_SERVER_H

#include "base_database_server.h"
#include "repeat_event_server.h"
#include "utils/event.h"


namespace vanity {

/*
 * An AutoExpiryDatabaseServer automatically emits expire events
 */
class AutoExpiryDatabaseServer:
	public virtual BaseDatabaseServer,
	public virtual RepeatEventServer
{
private:
	// time between automatic emitted server_events in microseconds
	static constexpr long M_EXPIRE_INTERVAL = 10 * 1000 * 1000;

public:
	// create an AutoExpiryDatabaseServer
	AutoExpiryDatabaseServer();

	// an expire event was received
	void event_expire() override;
};

} // namespace vanity

#endif //VANITY_AUTO_EXPIRY_DATABASE_SERVER_H

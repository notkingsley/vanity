//
// Created by kingsli on 12/4/23.
//

#ifndef VANITY_AUTO_EXPIRY_DATABASE_SERVER_H
#define VANITY_AUTO_EXPIRY_DATABASE_SERVER_H

#include "base_database_server.h"
#include "event.h"


namespace vanity {

/*
 * An AutoExpiryDatabaseServer automatically emits expire events
 */
class AutoExpiryDatabaseServer: public virtual BaseDatabaseServer
{
private:
	// time between automatic emitted server_events in microseconds
	static constexpr long M_EXPIRE_INTERVAL = 10 * 1000 * 1000;

	// whether the auto expiry has been stopped
	event m_stopped;

	// the expire_loop thread
	std::thread m_expire_loop_thread;

public:
	// start emitting expire events
	void start();

	// stop emitting expire events
	void stop();

	// erase expired keys
	void expire();

private:
	// run in a separate thread, yielding server_event::expire
	// to the event queue every M_EXPIRE_INTERVAL microseconds
	void expire_loop();
};

} // namespace vanity

#endif //VANITY_AUTO_EXPIRY_DATABASE_SERVER_H

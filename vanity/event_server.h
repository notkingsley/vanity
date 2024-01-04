//
// Created by kingsli on 12/23/23.
//

#ifndef VANITY_EVENT_SERVER_H
#define VANITY_EVENT_SERVER_H

#include "abstract_server.h"
#include "utils/queue.h"


namespace vanity {

/*
 * An EventServer dispatches events from a queue
 */
class EventServer : public virtual AbstractServer
{
protected:
	// some type of sever event that should be completed
	enum class server_event{
		socket_ready,
		persist,
		terminate,
		expire,
		publish,
	};

private:
	// events that need attention are sent through here
	queue<server_event> m_event_queue;

	// the event loop
	void event_loop();

public:
	// push an event onto the queue
	void push_event(server_event event);

	// process events from the queue until a terminate event is received
	void run();

	// request to terminate the server
	void terminate() override;

	// a socket_ready event was received
	virtual void event_socket_ready() = 0;

	// a persist event was received
	virtual void event_persist() = 0;

	// an expire event was received
	virtual void event_expire() = 0;

	// a publish event was received
	virtual void event_publish() = 0;
};

} // namespace vanity

#endif //VANITY_EVENT_SERVER_H

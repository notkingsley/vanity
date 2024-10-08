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
		epoll_ready,
		persist,
		terminate,
		expire,
		publish,
		pulse,
	};

private:
	// number of threads to use
	static constexpr int M_THREADS = 4;

	// events that need attention are sent through here
	Queue<server_event> m_event_queue;

	// the event loop
	void event_loop();

public:
	// push an event onto the queue
	void push_event(server_event event);

	// process events from the queue until a terminate event is received
	void run();

	// request to terminate the server
	void terminate() override;

	// an epoll_ready event was received
	virtual void event_epoll_ready() = 0;

	// a persist event was received
	virtual void event_persist() = 0;

	// an expire event was received
	virtual void event_expire() = 0;

	// a publish event was received
	virtual void event_publish() = 0;

	// a pulse event was received
	virtual void event_pulse() = 0;
};

} // namespace vanity

#endif //VANITY_EVENT_SERVER_H

//
// Created by kingsli on 1/21/24.
//

#ifndef VANITY_REPEAT_EVENT_SERVER_H
#define VANITY_REPEAT_EVENT_SERVER_H

#include "event_server.h"
#include "utils/event.h"

namespace vanity {

/*
 * A RepeatEventServer allows an event to be
 * repeatedly pushed after some constant time interval
 */
class RepeatEventServer : public virtual EventServer
{
private:
	using steady_clock = std::chrono::steady_clock;
	using microseconds = std::chrono::microseconds;

	// a repeated event
	struct repeated_event {
		steady_clock::time_point next;
		microseconds interval;
		server_event event;

		bool operator<(const repeated_event& other) const {
			return next > other.next;
		}
	};

	// registered repeat events
	std::priority_queue<repeated_event> m_repeat_events;

	// whether the repeat events have been stopped
	Event m_stopped;

	// the repeat_loop thread
	std::thread m_repeat_loop_thread;

	// loop to repeat events
	void repeat_loop();

	// reset next time_points for all events
	void reset_events();

	// get the current time
	static steady_clock::time_point now(){
		return steady_clock::now();
	};

protected:
	// register an event to be repeated every interval microseconds
	void repeat(server_event event, microseconds interval);

	// register an event to be repeated every interval microseconds
	void repeat(server_event event, long interval);

public:
	// start repeating events
	void start();

	// stop repeating events
	void stop();
};

} // namespace vanity
#endif //VANITY_REPEAT_EVENT_SERVER_H

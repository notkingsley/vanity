//
// Created by kingsli on 1/21/24.
//

#include "repeat_event_server.h"

namespace vanity {

void RepeatEventServer::repeat(server_event event, microseconds interval) {
	m_repeat_events.push({now() + interval, interval, event});
}

void RepeatEventServer::repeat(server_event event, long interval) {
	repeat(event, microseconds(interval));
}

void RepeatEventServer::repeat_loop() {
	if (m_repeat_events.empty())
		return;

	reset_events();
	while (true) {
		auto [next, interval, event] = m_repeat_events.top();
		m_repeat_events.pop();

		auto sleep_time = next - now();
		if (sleep_time.count() < 0)
			sleep_time = microseconds(0);

		using std::chrono::duration_cast;
		if (m_stopped.wait(duration_cast<microseconds>(sleep_time).count()))
			return;

		push_event(event);
		m_repeat_events.emplace(next + interval, interval, event);
	}
}

void RepeatEventServer::start() {
	m_stopped.clear();
	m_repeat_loop_thread = std::thread(&RepeatEventServer::repeat_loop, this);
}

void RepeatEventServer::stop() {
	m_stopped.set();
	m_repeat_loop_thread.join();
}

void RepeatEventServer::reset_events() {
	std::priority_queue<repeated_event> new_events;
	auto now = steady_clock::now();

	while (not m_repeat_events.empty()) {
		auto [next, interval, event] = m_repeat_events.top();
		m_repeat_events.pop();
		new_events.emplace(now + interval, interval, event);
	}

	m_repeat_events.swap(new_events);
}

} // namespace vanity
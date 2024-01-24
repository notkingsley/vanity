//
// Created by kingsli on 10/2/23.
//

#ifndef VANITY_EVENT_H
#define VANITY_EVENT_H

#include <condition_variable>
#include <mutex>


namespace vanity
{

// Event is a class that can be used to synchronize threads
class Event
{
private:
	std::condition_variable m_cond;
	std::mutex m_mutex;
	bool m_state = false;

public:
	// default constructor
	Event() = default;

	// move constructor
	Event(Event&& other) noexcept{
		m_state = other.m_state; // copy state
	}

	// move assignment
	Event& operator=(Event&& other) noexcept{
		std::lock_guard lock{m_mutex};
		m_state = other.m_state; // copy state
		return *this;
	}

	// check if the event is set
	bool is_set() const {
		return m_state;
	}
	
	// set the event
	void set(){
		{
			std::lock_guard lock{m_mutex};
			m_state = true;
		}
		m_cond.notify_all();
	}

	// clear the event
	void clear() {
		std::lock_guard lock{m_mutex};
		m_state = false;
	}

	// wait indefinitely for the event to be set
	void wait() {
		std::unique_lock<std::mutex> lock(m_mutex);
		while (not is_set())
			m_cond.wait(lock);
	}

	// wait for event or timeout in microseconds
	// return false on timeout, true otherwise
	bool wait(long timeout) {
		using namespace std::chrono;
		auto end = steady_clock::now() + microseconds{timeout};
		std::unique_lock<std::mutex> lock {m_mutex};
		return m_cond.wait_until(lock, end, [&](){return this->is_set();});
	}
};

} // namespace vanity

#endif //VANITY_EVENT_H

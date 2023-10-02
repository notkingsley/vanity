//
// Created by kingsli on 10/2/23.
//

#ifndef VANITY_EVENT_H
#define VANITY_EVENT_H

#include <condition_variable>
#include <mutex>


namespace vanity
{

// event is a class that can be used to synchronize threads
class event
{
private:
	std::condition_variable m_cond;
	std::mutex m_mutex;
	bool m_state = false;

public:
	// check if the event is set
	bool is_set() {
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

	// wait for the event to be set
	void wait() {
		std::unique_lock<std::mutex> lock(m_mutex);
		while (not is_set())
			m_cond.wait(lock);
	}
};

} // namespace vanity

#endif //VANITY_EVENT_H
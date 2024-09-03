//
// Created by kingsli on 8/27/24.
//

#ifndef VANITY_ATOMIC_COUNTER_H
#define VANITY_ATOMIC_COUNTER_H

#include <mutex>

namespace vanity {

/*
 * An AtomicCounter is a thread-safe counter
 */
class AtomicCounter
{
private:
	// the counter value
	uint64_t m_value;

	// the mutex for the counter
	std::mutex m_mutex;

public:
	// create a new AtomicCounter with an initial value
	explicit AtomicCounter(uint64_t value) : m_value(value) {}

	// get the next value of the counter
	uint64_t next() {
		std::lock_guard lock(m_mutex);
		return m_value++;
	}
};

} // namespace vanity

#endif //VANITY_ATOMIC_COUNTER_H

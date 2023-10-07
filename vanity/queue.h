//
// Created by kingsli on 10/2/23.
//

#ifndef VANITY_QUEUE_H
#define VANITY_QUEUE_H

#include <condition_variable>
#include <queue>
#include <mutex>


namespace vanity {

// a threadsafe queue.
template <class T>
class queue
{
private:
	std::queue<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cond;

public:
	// default constructor
	queue() = default;

	// move constructor
	queue(queue&& other) noexcept
	{
		std::scoped_lock lock{m_mutex, other.m_mutex};
		m_queue = std::move(other.m_queue);
	}

	// move assignment
	queue& operator=(queue&& other) noexcept
	{
		std::scoped_lock lock{m_mutex, other.m_mutex};
		m_queue = std::move(other.m_queue);
		return *this;
	}

	// get an element from the queue, blocking till one is available
	T get()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		while(m_queue.empty())
			m_cond.wait(lock);

		T val{std::move(m_queue.front())};
		m_queue.pop();
		return val;
	}

	// check if the queue is empty
	bool empty()
	{
		std::lock_guard lock(m_mutex);
		return m_queue.empty();
	}

	// push an item onto the queue
	void push(T item) {
		{
			std::lock_guard lock(m_mutex);
			m_queue.push(std::move(item));
		}
		m_cond.notify_one();
	}

	// emplace an item onto the queue
	template<typename ...Args>
	void emplace(Args&&... args)
	{
		{
			std::lock_guard lock(m_mutex);
			m_queue.emplace(std::forward<Args>(args)...);
		}
		m_cond.notify_one();
	}
};

} // namespace vanity

#endif //VANITY_QUEUE_H

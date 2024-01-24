//
// Created by kingsli on 10/16/23.
//

#ifndef VANITY_TASK_SERIALIZER_H
#define VANITY_TASK_SERIALIZER_H

#include <future>
#include <thread>

#include "queue.h"


namespace vanity {

/*
 * A TaskSerializer allows to serialize operations to a single thread
 * You most likely do NOT want to inherit this virtually: the same virtual base
 * class will serialize all tasks to the same thread
 */
template<typename task_type, typename data_type, typename ret_type>
class TaskSerializer
{
private:
	Queue<std::tuple<task_type, data_type, std::promise<ret_type>>> m_queue;
	std::thread m_run_thread;
	bool m_running {false};

public:
	TaskSerializer() = default;

	virtual ~TaskSerializer() = default;

	// send a task to the operation thread
	auto send_task(task_type&& t, data_type&& data)
	{
		std::promise<ret_type> promise;
		auto future = promise.get_future();
		m_queue.emplace(std::move(t), std::move(data), std::move(promise));
		return future;
	}

	// start the serializer thread.
	void start(long max_timeout) {
		m_running = true;
		m_run_thread = std::thread{&TaskSerializer::run, this, max_timeout};
	}

	// stop the serializer thread
	void stop() {
		m_running = false;
		m_run_thread.join();
	}

protected:
	// actually perform some task that was sent
	virtual void perform(task_type t, data_type data, std::promise<ret_type>) = 0;

private:
	// run the serial thread
	// wake every max_timeout to check if stopped
	void run(long max_timeout){
		while (true){
			if (m_queue.wait(max_timeout)){
				auto [t, data, promise] = m_queue.get();
				perform(std::move(t), std::move(data), std::move(promise));
			}
			if (not m_running)
				break;
		}
	}
};

} // namespace vanity

#endif //VANITY_TASK_SERIALIZER_H

//
// Created by kingsli on 10/16/23.
//

#ifndef VANITY_TASK_SERIALIZER_H
#define VANITY_TASK_SERIALIZER_H

#include <thread>

#include "queue.h"


namespace vanity {

/*
 * A TaskSerializer allows to serialize operations to a single thread
 * You most likely do NOT want to inherit this virtually: the same virtual base
 * class will serialize all tasks to the same thread
 */
template<typename task_type, typename data_type>
class TaskSerializer
{
private:
	queue<std::pair<task_type, data_type>> m_queue;
	std::thread m_run_thread;
	bool m_running {false};

public:
	TaskSerializer() = default;

	virtual ~TaskSerializer() = default;

	// send a task to the operation thread
	void send_task(task_type t, data_type&& data)
	{
		m_queue.emplace(t, std::move(data));
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
	virtual void perform(task_type t, data_type data) = 0;

private:
	// run the serial thread
	// wake every max_timeout to check if stopped
	void run(long max_timeout){
		while (true){
			if (m_queue.wait(max_timeout)){
				auto [t, data] = m_queue.get();
				perform(t, std::move(data));
			}
			if (not m_running)
				break;
		}
	}
};

} // namespace vanity

#endif //VANITY_TASK_SERIALIZER_H

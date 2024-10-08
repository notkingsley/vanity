//
// Created by kingsli on 10/7/23.
//

#ifndef VANITY_SOCKET_EPOLL_H
#define VANITY_SOCKET_EPOLL_H

#include "socket.h"
#include "socket_read_handler.h"
#include "socket_write_handler.h"


namespace vanity::socket {

/*
 * A base class for epoll objects
 */
class EpollBase
{
protected:
	// the epoll file descriptor
	int m_fd {};

public:
	// create an epoll
	EpollBase();

	// destroy the epoll
	~EpollBase();

	// no copy
	EpollBase(const EpollBase&) = delete;
	EpollBase& operator=(const EpollBase&) = delete;

	// move constructor
	EpollBase(EpollBase&& other) noexcept;
	EpollBase& operator=(EpollBase&& other) noexcept;

	// block until some socket is ready
	int wait(epoll_event* events, int max_events, int timeout) const;
};

/*
 * An epoll lets us poll for ready socket
 */
class Epoll : public EpollBase
{
private:
	friend class SuperEpoll;

	// add an object to the epoll
	void add(uint32_t event_mask, void* data_ptr, int fd) const;

	// remove an object from the epoll
	void remove(int fd) const;

public:
	// add an object to the epoll
	void add(SocketReadHandler& handler) const;

	// remove an object from the epoll
	void remove(const SocketReadHandler& handler) const;

	// add an object to the epoll
	void add(SocketWriteHandler& handler) const;

	// remove an object from the epoll
	void remove(const SocketWriteHandler& handler) const;
};

/*
 * An epoll that can poll for other ready epoll
 */
class SuperEpoll : public EpollBase
{
public:
	// add an epoll to the epoll
	void add(Epoll& epoll) const;

	// remove an epoll from the epoll
	void remove(Epoll& epoll) const;
};

} // namespace vanity::socket

#endif //VANITY_SOCKET_EPOLL_H

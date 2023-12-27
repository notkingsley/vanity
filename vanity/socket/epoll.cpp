//
// Created by kingsli on 10/7/23.
//

#include <unistd.h>

#include "epoll.h"

namespace vanity {

EpollBase::EpollBase() {
	m_fd = epoll_create1(0);
	if (m_fd < 0)
		throw SocketError("Could not create epoll");
}

EpollBase::~EpollBase() {
	if (m_fd >= 0)
		close(m_fd);
}

EpollBase::EpollBase(EpollBase &&other) noexcept {
	m_fd = other.m_fd;
	other.m_fd = -1;
}

EpollBase &EpollBase::operator=(EpollBase &&other) noexcept {
	m_fd = other.m_fd;
	other.m_fd = -1;
	return *this;
}

int EpollBase::wait(epoll_event *events, int max_events, int timeout) const {
	return epoll_wait(m_fd, events, max_events, timeout);
}


void Epoll::add(SocketEventHandler& handler) const {
	epoll_event event{};
	event.events = handler.get_event_mask();
	event.data.ptr = &handler;
	int ctl = epoll_ctl(m_fd, EPOLL_CTL_ADD, handler.socket_fd(), &event);
	if (ctl < 0)
		throw SocketError("Could not add client to epoll");
}

void Epoll::remove(const SocketEventHandler &handler) const {
	int ctl = epoll_ctl(m_fd, EPOLL_CTL_DEL, handler.socket_fd(), nullptr);
	if (ctl < 0)
		throw SocketError("Could not remove client from epoll");
}


void SuperEpoll::add(Epoll &epoll) const {
	epoll_event event{};
	event.events = EPOLLIN;
	event.data.ptr = &epoll;
	int ctl = epoll_ctl(m_fd, EPOLL_CTL_ADD, epoll.m_fd, &event);
	if (ctl < 0)
		throw SocketError("Could not add epoll to epoll");
}

void SuperEpoll::remove(Epoll &epoll) const {
	int ctl = epoll_ctl(m_fd, EPOLL_CTL_DEL, epoll.m_fd, nullptr);
	if (ctl < 0)
		throw SocketError("Could not remove epoll from epoll");
}

} // namespace vanity
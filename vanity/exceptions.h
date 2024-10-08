//
// Created by kingsli on 10/20/23.
//

#ifndef VANITY_EXCEPTIONS_H
#define VANITY_EXCEPTIONS_H

#include <cstring>
#include <string>


namespace vanity {

/*
 * common base class for all exceptions
 */
class Exception : public std::exception
{
public:
	Exception() = default;
	~Exception() override = default;
	
	Exception(const Exception&) = default;
};

/*
There was a connection related error
*/
class SocketError : public Exception
{
private:
	std::string m_msg {"SocketError: "};
	int m_errno;

public:
	explicit SocketError(const std::string& msg) : m_errno{errno} {
		m_msg += msg + " errno: " + strerror(m_errno);
	}

	// get the error message
	const char* what() const noexcept override {
		return m_msg.c_str();
	}

	// get the errno
	int get_errno() const noexcept {
		return m_errno;
	}

	// check if the error is an EAGAIN
	bool is_retry() const noexcept {
		return m_errno == EAGAIN;
	}

	// check if the error was an interrupt
	bool is_interrupt() const noexcept {
		return m_errno == EINTR;
	}
};

// the message received was invalid
class InvalidRequest : Exception
{
private:
	std::string m_msg {"InvalidRequest: "};

public:
	explicit InvalidRequest(const std::string& msg){
		m_msg += msg;
	}

	const char* what() const noexcept override {
		return m_msg.c_str();
	}
};

// thrown when an error occurs in the hashing process
class HashError : public Exception {
	std::string m_msg {"HashError: "};

public:
	explicit HashError(const std::string& msg) {
		m_msg += msg;
	};

	const char *what() const noexcept override {
		return m_msg.c_str();
	}
};

/*
 * Thrown for malformed arguments
 */
class MalformedArgument : public Exception
{
private:
	std::string m_msg;

public:
	explicit MalformedArgument(std::string msg) : m_msg{std::move(msg)} {}
	const char* what() const noexcept override { return m_msg.c_str(); }
};

/*
 * Thrown when an invalid Journal is being read
 */
class CorruptJournal: public Exception
{
private:
	std::string m_msg {"CorruptJournal: "};

public:
	explicit CorruptJournal(const std::string& msg) {
		m_msg += msg;
	}

	const char* what() const noexcept override {
		return m_msg.c_str();
	}
};

/*
 * Thrown when the FileLock fails to acquire a lock
 */
class FileLockError : public Exception
{
private:
	std::string m_msg {"FileLockError: "};

public:
	explicit FileLockError(const std::string& msg) {
		m_msg += msg;
	}

	const char* what() const noexcept override {
		return m_msg.c_str();
	}
};

/*
 * Thrown when the Logger encounters an error
 */
class LogError : public Exception
{
private:
	std::string m_msg {"LogError: "};
	
public:
	explicit LogError(const std::string& msg) {
		m_msg += msg;
	}

	const char* what() const noexcept override {
		return m_msg.c_str();
	}
};

/*
 * Thrown when a WAL invariant is violated
 */
class WALError : public Exception
{
private:
	std::string m_msg {"WALError: "};

public:
	explicit WALError(const std::string& msg) {
		m_msg += msg;
	}

	const char* what() const noexcept override {
		return m_msg.c_str();
	}
};

} // namespace vanity

#endif //VANITY_EXCEPTIONS_H

//
// Created by kingsli on 10/20/23.
//

#ifndef VANITY_EXCEPTIONS_H
#define VANITY_EXCEPTIONS_H

namespace vanity {

/*
 * common base class for all exceptions
 */
class Exception : public std::exception {};

/*
There was a connection related error
*/
class SocketError : public Exception
{
private:
	std::string m_msg {" SocketError: "};
	int m_errno;

public:
	explicit SocketError(const std::string& msg) : m_errno{errno} {
		m_msg += msg + " errno: " + std::to_string(m_errno);
	}

	const char* what() const noexcept override { return m_msg.c_str(); }

	int get_errno() const noexcept { return m_errno; }
};

// thrown from a ready() to indicate the client should be disconnected
class DestroyClient : Exception {};

// the message received was invalid
class InvalidRequest : Exception
{
private:
	std::string m_msg{" InvalidRequest: "};

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
	std::string m_msg{" HashError: "};

public:
	explicit HashError(const std::string& msg) {
		m_msg += msg;
	};

	const char *what() const noexcept override {
		return m_msg.c_str();
	}
};

} // namespace vanity

#endif //VANITY_EXCEPTIONS_H

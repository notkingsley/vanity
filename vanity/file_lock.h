//
// Created by kingsli on 3/27/24.
//

#ifndef VANITY_FILE_LOCK_H
#define VANITY_FILE_LOCK_H

#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <optional>
#include <unistd.h>

#include "exceptions.h"


namespace vanity {

/*
 * A file lock to optionally acquire a lock on a file
 * If the file is already locked, the constructor will throw a FileLockError
 * If the file is locked, the destructor will unlock the file
 */
class FileLock
{
private:
	using path = std::filesystem::path;

	// the file to lock
	// if not set, the lock is not acquired
	std::optional<path> m_file;

	// the file descriptor
	int m_fd = -1;

	// writes the current process id to the lock file
	// throws a FileLockError if the write fails
	static void write_pid(const path& file) {
		auto pid = getpid();
		std::ofstream os{file, std::ios::out};
		if (os)
			os << pid << std::flush;
		else
			throw_error("Failed to write to lock file: ");
	}

	// check if a process is alive
	static bool is_alive(pid_t pid) {
		return kill(pid, 0) == 0;
	}

	// deletes the lock file if it exists
	// but the owner is dead
	static void delete_if_owner_dead(const path& file) {
		std::ifstream ifs{file, std::ios::in};
		if (not ifs)
			return;

		pid_t pid;
		if (ifs >> pid and is_alive(pid))
			return;

		ifs.close();
		remove(file);
	}

	// throws a FileLockError starting with the given message
	[[noreturn]] static void throw_error(const char* msg) {
		throw FileLockError{msg + std::string{strerror(errno)}};
	}

public:
	// create a lock_file lock
	explicit FileLock(std::optional<path> lock_file) : m_file{std::move(lock_file)} {
		if (not m_file)
			return;

		auto& file = *m_file;
		delete_if_owner_dead(file);

		m_fd = open(file.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0666);
		if (m_fd == -1)
			if (errno == EEXIST)
				throw_error("Lock is already acquired: ");
			else
				throw_error("Failed to acquire lock: ");
		else
			write_pid(file);
	}

	// no copy
	FileLock(const FileLock&) = delete;
	FileLock& operator=(const FileLock&) = delete;

	// no move
	FileLock(FileLock&&) = delete;
	FileLock& operator=(FileLock&&) = delete;

	// return true if the lock is acquired
	explicit operator bool() const {
		return m_file.has_value();
	}

	// release the lock
	~FileLock() {
		if (m_fd != -1)
			close(m_fd);

		if (m_file)
			remove(*m_file);
	}
};

} // namespace vanity

#endif //VANITY_FILE_LOCK_H

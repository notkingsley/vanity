//
// Created by kingsli on 3/27/24.
//

#ifndef VANITY_FILE_LOCK_H
#define VANITY_FILE_LOCK_H

#include <cstring>
#include <fcntl.h>
#include <filesystem>
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
	// the file to lock
	// if not set, the lock is not acquired
	std::optional<std::filesystem::path> m_file;

	// the file descriptor
	int m_fd = -1;

public:
	// create a file lock
	explicit FileLock(std::optional<std::filesystem::path> file) : m_file{std::move(file)}
	{
		if (not m_file)
			return;

		m_fd = open(m_file->c_str(), O_WRONLY | O_CREAT | O_EXCL, 0666);
		if (m_fd == -1 and errno == EEXIST)
			throw FileLockError{"Lock is already acquired"};

		if (m_fd == -1)
			throw FileLockError{"Failed to acquire lock: " + std::string{strerror(errno)}};
	}

	// no copy
	FileLock(const FileLock&) = delete;
	FileLock& operator=(const FileLock&) = delete;

	// no move
	FileLock(FileLock&&) = delete;
	FileLock& operator=(FileLock&&) = delete;

	// return true if the lock is acquired
	explicit operator bool() const
	{
		return m_file.has_value();
	}

	// release the lock
	~FileLock()
	{
		if (m_fd != -1)
			close(m_fd);

		if (m_file)
			remove(*m_file);
	}
};

} // namespace vanity

#endif //VANITY_FILE_LOCK_H

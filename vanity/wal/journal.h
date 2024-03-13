//
// Created by kingsli on 3/11/24.
//

#ifndef VANITY_JOURNAL_H
#define VANITY_JOURNAL_H

#include <filesystem>
#include <optional>
#include <fstream>

#include "exceptions.h"
#include "utils/serializer.h"


namespace vanity::journal {

using path = std::filesystem::path;

/*
 * One of the possible entries in the journal
 */
enum JournalState
{
	EMPTY_JOURNAL,
	DB_FILE_NO_EXIST,
	DB_FILE_EXIST,
	MOVING_EXISTING_DB_FILE,
	MOVED_EXISTING_DB_FILE,
	MOVED_NEW_DB_FILE,
};

/*
 * A Journal wraps a file stream and the associated file
 * It writes JournalState to the file, without any restrictions
 * on the validity of the Journal entries
 */
class Journal
{
private:
	// the file stream
	std::ofstream m_journal;

	// the associated file
	path m_journal_file;

public:
	// create a journal
	explicit Journal(const path& journal_file);

	// write a JournalState to the journal
	void journal(JournalState state);

	// write a JournalState and a path to the journal
	void journal(JournalState state, const path& existing_db_file);

	// close the journal
	void close();

	// delete the journal
	void wipe();
};

// the hierarchy of JournalMaker classes help enforce the correct
// order of journal entries
template<JournalState state>
class JournalMaker;

template<>
class JournalMaker<MOVED_NEW_DB_FILE>
{
private:
	// the journal file
	Journal m_journal;

	// create a journal maker
	explicit JournalMaker(Journal journal) : m_journal(std::move(journal)) {}

	friend class JournalMaker<MOVED_EXISTING_DB_FILE>;

public:
	// delete the journal
	void journal_delete();
};

template<>
class JournalMaker<MOVED_EXISTING_DB_FILE>
{
private:
	// the journal file
	Journal m_journal;

	// create a journal maker
	explicit JournalMaker(Journal journal) : m_journal(std::move(journal)) {}

	friend class JournalMaker<MOVING_EXISTING_DB_FILE>;

public:
	// write a MOVED_NEW_DB_FILE state to the journal
	// returns a JournalMaker<MOVED_NEW_DB_FILE>
	JournalMaker<MOVED_NEW_DB_FILE> journal_moved_new_db_file();
};

template<>
class JournalMaker<MOVING_EXISTING_DB_FILE>
{
private:
	// the journal file
	Journal m_journal;

	// create a journal maker
	explicit JournalMaker(Journal journal) : m_journal(std::move(journal)) {}

	friend class JournalMaker<DB_FILE_EXIST>;

public:
	// write a MOVED_EXISTING_DB_FILE state to the journal and close it
	// returns a JournalMaker<MOVED_EXISTING_DB_FILE>
	JournalMaker<MOVED_EXISTING_DB_FILE> journal_moved_existing_db_file();
};

template<>
class JournalMaker<DB_FILE_EXIST>
{
private:
	// the journal file
	Journal m_journal;

	// create a journal maker
	explicit JournalMaker(Journal journal) : m_journal(std::move(journal)) {}

	friend class JournalMaker<EMPTY_JOURNAL>;

public:
	// write a MOVING_EXISTING_DB_FILE state to the journal
	// returns a JournalMaker<MOVING_EXISTING_DB_FILE>
	JournalMaker<MOVING_EXISTING_DB_FILE> journal_moving_existing_db_file(const path& old_db_file);
};

template<>
class JournalMaker<DB_FILE_NO_EXIST>
{
private:
	// the journal file
	Journal m_journal;

	// create a journal maker
	explicit JournalMaker(Journal journal) : m_journal(std::move(journal)) {}

	friend class JournalMaker<EMPTY_JOURNAL>;

public:
	// delete the journal
	void journal_delete();
};

template<>
class JournalMaker<EMPTY_JOURNAL>
{
	// the journal file
	Journal m_journal;

	// create a journal maker
	explicit JournalMaker(const path& journal_file) : m_journal(journal_file) {}

	friend JournalMaker<EMPTY_JOURNAL> new_journal_maker(const path& journal_file);

public:
	// write a DB_FILE_NO_EXIST state to the journal
	// this closes the journal after writing
	JournalMaker<DB_FILE_NO_EXIST> journal_db_file_no_exist();

	// write a DB_FILE_EXIST state to the journal
	// returns a JournalMaker<DB_FILE_EXIST>
	JournalMaker<DB_FILE_EXIST> journal_db_file_exist();
};

// create a new JournalMaker<EMPTY_JOURNAL>
JournalMaker<EMPTY_JOURNAL> new_journal_maker(const path& journal_file);

class RecoveredJournal
{
private:
	// the state of the journal
	JournalState m_state {EMPTY_JOURNAL};

	// associated path with MOVING_EXISTING_DB_FILE, if any
	std::optional<path> m_moving_existing_db_file;

	// check if the ifstream is at the end of the file
	static bool at_eof(std::ifstream& in) {
		return in.peek() == std::ifstream::traits_type::eof();
	}

public:
	// create a recovered journal from a journal file
	explicit RecoveredJournal(const path& journal_file);

	// get the state of the journal
	JournalState get_state() const;

	// get the associated path with MOVING_EXISTING_DB_FILE, if any
	// throws bad optional access if there is no associated path
	const path& get_existing_db_file() const;
};

} // namespace vanity::journal

#endif //VANITY_JOURNAL_H

//
// Created by kingsli on 3/11/24.
//

#include "journal.h"

namespace vanity::wal::journal {

Journal::Journal(const path &journal_file) {
	m_journal_data = std::make_unique<journal_data>(journal_file, journal_file);
}

void Journal::journal(JournalState state) {
	serializer::write(m_journal_data->file, state);
	m_journal_data->file.flush();
}

void Journal::journal(JournalState state, const path &existing_db_file) {
	serializer::write(m_journal_data->file, state);
	serializer::write(m_journal_data->file, absolute(existing_db_file).string());
	m_journal_data->file.flush();
}

void Journal::close() {
	m_journal_data->file.close();
}

void Journal::wipe() {
	close();
	remove(m_journal_data->file_path);
}

void JournalMaker<MOVED_NEW_DB_FILE>::delete_() {
	m_journal.wipe();
}

JournalMaker<MOVED_NEW_DB_FILE>
JournalMaker<MOVED_EXISTING_DB_FILE>::moved_new_db_file() {
	m_journal.journal(MOVED_NEW_DB_FILE);
	m_journal.close();
	return JournalMaker<MOVED_NEW_DB_FILE>{std::move(m_journal)};
}

JournalMaker<MOVED_EXISTING_DB_FILE>
JournalMaker<MOVING_EXISTING_DB_FILE>::moved_existing_db_file() {
	m_journal.journal(MOVED_EXISTING_DB_FILE);
	return JournalMaker<MOVED_EXISTING_DB_FILE>{std::move(m_journal)};
}

JournalMaker<MOVING_EXISTING_DB_FILE>
JournalMaker<DB_FILE_EXIST>::moving_existing_db_file(const path& old_db_file) {
	m_journal.journal(MOVING_EXISTING_DB_FILE, old_db_file);
	return JournalMaker<MOVING_EXISTING_DB_FILE>{std::move(m_journal)};
}

void JournalMaker<DB_FILE_NO_EXIST>::delete_() {
	m_journal.wipe();
}

JournalMaker<DB_FILE_NO_EXIST>
JournalMaker<EMPTY_JOURNAL>::db_file_no_exist() {
	m_journal.journal(DB_FILE_NO_EXIST);
	m_journal.close();
	return JournalMaker<DB_FILE_NO_EXIST>{std::move(m_journal)};
}

JournalMaker<DB_FILE_EXIST>
JournalMaker<EMPTY_JOURNAL>::db_file_exist() {
	m_journal.journal(DB_FILE_EXIST);
	return JournalMaker<DB_FILE_EXIST>{std::move(m_journal)};
}

JournalMaker<EMPTY_JOURNAL>
new_journal_maker(const path& journal_file) {
	return JournalMaker<EMPTY_JOURNAL>{journal_file};
}

RecoveredJournal::RecoveredJournal(const path &journal_file) {
	if (not exists(journal_file))
		return;

	std::ifstream in{journal_file};
	if (at_eof(in))
		return;

	m_state = serializer::read<JournalState>(in);
	if (m_state == DB_FILE_NO_EXIST) {
		if (at_eof(in))
			return;
		throw CorruptJournal{"Journal is corrupted: extra data after DB_FILE_NO_EXIST"};
	}

	if (m_state != DB_FILE_EXIST)
		throw CorruptJournal{"Journal is corrupted: expected DB_FILE_EXIST or DB_FILE_NO_EXIST"};

	if (at_eof(in))
		return;

	m_state = serializer::read<JournalState>(in);
	if (m_state != MOVING_EXISTING_DB_FILE)
		throw CorruptJournal{"Journal is corrupted: expected MOVING_EXISTING_DB_FILE"};

	m_moving_existing_db_file = serializer::read<std::string>(in);

	if (at_eof(in))
		return;

	m_state = serializer::read<JournalState>(in);
	if (m_state != MOVED_EXISTING_DB_FILE)
		throw CorruptJournal{"Journal is corrupted: expected MOVED_EXISTING_DB_FILE"};

	if (at_eof(in))
		return;

	m_state = serializer::read<JournalState>(in);
	if (m_state != MOVED_NEW_DB_FILE)
		throw CorruptJournal{"Journal is corrupted: expected MOVED_NEW_DB_FILE"};

	if (not at_eof(in))
		throw CorruptJournal{"Journal is corrupted: extra data after MOVED_NEW_DB_FILE"};
}

JournalState RecoveredJournal::get_state() const {
	return m_state;
}

const path &RecoveredJournal::get_existing_db_file() const {
	return *m_moving_existing_db_file;
}

bool RecoveredJournal::at_eof(std::ifstream &in) {
	return in.peek() == std::ifstream::traits_type::eof();
}

} // namespace vanity::wal::journal
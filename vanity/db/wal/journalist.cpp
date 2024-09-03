//
// Created by kingsli on 8/11/24.
//

#include "journalist.h"

namespace vanity::wal {

// return a path with the same parent as file, but with the given prefix
auto with_name_prefix(const std::filesystem::path& file, const char* prefix) {
	return file.parent_path() / (prefix + file.filename().string());
}

Journalist::path Journalist::make_temp_db_file(const path &db_file) {
	return with_name_prefix(db_file, "tmp.");
}

Journalist::path Journalist::make_old_db_file(const path &db_file) {
	return with_name_prefix(db_file, "old.");
}

void Journalist::journal_db_file_no_exist() {
	auto no_exist = m_empty_journal.db_file_no_exist();
	rename(m_tmp_db_file, m_db_file);
	remove(m_wal_file);
	no_exist.delete_();
}

void Journalist::journal_db_file_exist() {
	auto old_db_file = make_old_db_file(m_db_file);
	auto exist = m_empty_journal.db_file_exist();
	auto moving = exist.moving_existing_db_file(old_db_file);
	rename(m_db_file, old_db_file);
	auto moved = moving.moved_existing_db_file();
	rename(m_tmp_db_file, m_db_file);
	auto moved_new = moved.moved_new_db_file();
	remove(m_wal_file);
	moved_new.delete_();
	remove(old_db_file);
}

Journalist::Journalist(const path &journal_file, const path &db_file, const path &wal_file):
	m_db_file{db_file},
	m_tmp_db_file{make_temp_db_file(db_file)},
	m_wal_file{wal_file},
	m_empty_journal{journal::new_journal_maker(journal_file)}
{}

void Journalist::switch_and_journal() {
	if (exists(m_db_file))
		journal_db_file_exist();
	else
		journal_db_file_no_exist();
}

const Journalist::path &Journalist::tmp_db_file() const {
	return m_tmp_db_file;
}

} // namespace vanity::wal

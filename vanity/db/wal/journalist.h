//
// Created by kingsli on 8/11/24.
//

#ifndef VANITY_JOURNALIST_H
#define VANITY_JOURNALIST_H

#include "journal.h"

namespace vanity::wal {

/*
 * An uncreative name, to tell the truth.
 * A Journalist handles the logic for journaling a persist operation
 * This is one-use only: one Journalist for one persist operation
 *
 * You create a Journalist, perform the persist operation, and then call switch_and_journal()
 */
class Journalist
{
private:
	using path = std::filesystem::path;

	using empty_journal_t = std::invoke_result_t<decltype(journal::new_journal_maker), path>;

	// the database file
	const path &m_db_file;

	// the temporary database file
	path m_tmp_db_file;

	// the WAL file
	const path &m_wal_file;

	// the empty journal
	empty_journal_t m_empty_journal;

	// return the path to a temporary database file
	// the path returned must be distinct from db_file
	static path make_temp_db_file(const path &db_file);

	// return the path to the old database file
	// the path returned must be distinct from db_file
	static path make_old_db_file(const path &db_file);

	// journal when the database file does not exist
	void journal_db_file_no_exist();

	// journal when the database file already exists
	void journal_db_file_exist();

public:
	// create a journalist
	Journalist(const path &journal_file, const path &db_file, const path &wal_file);

	// journal the persist operation
	void switch_and_journal();

	// get the temporary database file
	const path &tmp_db_file() const;
};

} // namespace vanity::wal

#endif //VANITY_JOURNALIST_H

//
// Created by kingsli on 3/15/24.
//

#ifndef VANITY_WAL_ENTRY_T_H
#define VANITY_WAL_ENTRY_T_H

namespace vanity::wal {

enum class wal_entry_t : char {
	db_op,
	expire,
};

} // namespace vanity::wal

#endif //VANITY_WAL_ENTRY_T_H

project(vanity_db_wal)

include_directories(.)

add_library(vanity_db_wal STATIC
	journal.cpp
	journalist.cpp
	persist_journal_server.cpp
	write_ahead_logger.cpp
)

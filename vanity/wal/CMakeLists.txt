project(vanity_wal)

include_directories(.)

add_library(vanity_wal STATIC
	journal.cpp
	journalist.cpp
	persist_journal_server.cpp
	write_ahead_logger.cpp
)

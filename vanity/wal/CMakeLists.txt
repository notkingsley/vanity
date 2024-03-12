project(vanity_wal)

include_directories(.)

add_library(vanity_wal STATIC
	journal.cpp
	persist_journal_server.cpp
	wal_recovery_client.cpp
	wal_recovery_server.cpp
	wal_server.cpp
	wal_write_server.cpp
)

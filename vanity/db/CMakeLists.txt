project(vanity_db)

include_directories(..)

add_subdirectory(db)
add_subdirectory(servers)
add_subdirectory(wal)

add_library(vanity_db STATIC
	transaction_server.cpp
)

target_link_libraries(
	vanity_db
	vanity_db_db
	vanity_db_servers
	vanity_db_wal
)
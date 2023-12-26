project(vanity_db)

include_directories(..)

add_subdirectory(./db)
add_subdirectory(./servers)

add_library(vanity_db STATIC
	./servers/database_server.h
)

target_link_libraries(
	vanity_db
	vanity_db_db
	vanity_db_servers
)
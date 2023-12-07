project(vanity_db)

include_directories(.)

add_subdirectory(db)

add_library(vanity_db STATIC
	auto_expiry_database_server.cpp
	base_database_server.cpp
	expiry_database_server.cpp
	hash_database_server.cpp
	list_database_server.cpp
	persistent_server.cpp
	primitive_database_server.cpp
	set_database_server.cpp
)

target_link_libraries(
	vanity_db
	vanity_db_db
)
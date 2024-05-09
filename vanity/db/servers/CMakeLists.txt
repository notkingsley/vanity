project(vanity_db_servers)

include_directories(.)

add_library(vanity_db_servers STATIC
	base_database_server.cpp
	expiry_database_server.cpp
	hash_database_server.cpp
	list_database_server.cpp
	primitive_database_server.cpp
	set_database_server.cpp
)
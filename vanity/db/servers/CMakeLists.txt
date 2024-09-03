project(vanity_db_servers)

include_directories(.)

add_library(vanity_db_servers STATIC
	database_object_server.cpp
	# linker error if this is not in this file
	database_wrapper.cpp
	expiry_database_server.cpp
	hash_database_server.cpp
	list_database_server.cpp
	primitive_database_server.cpp
	set_database_server.cpp
	general_database_server.cpp
	general_database_server.h
)
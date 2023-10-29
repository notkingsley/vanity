project(vanity_db)

include_directories(.)

add_library(vanity_db STATIC
	base_database_server.cpp
    database.cpp
	list_database_server.cpp
	persistent_server.cpp
	primitive_database_server.cpp
	serial_database.cpp
	base_database.cpp
	base_database.h
	primitive_database.cpp
	primitive_database.h
)
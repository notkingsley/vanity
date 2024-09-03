project(vanity_db_db)

include_directories(.)

add_library(vanity_db_db STATIC
	base_database.cpp
    database.cpp
	expiry_database.cpp
	hash_database.cpp
	list_database.cpp
	locked_database.cpp
	primitive_database.cpp
	set_database.cpp
	general_database.cpp
	general_database.h
)
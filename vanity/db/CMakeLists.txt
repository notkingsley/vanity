project(vanity_db)

include_directories(.)

add_library(vanity_db STATIC
    database.cpp
    database_server.cpp
	persistent_server.cpp
)
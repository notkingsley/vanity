project(vanity_db)

include_directories(.)

add_library(vanity_db STATIC
    database.cpp
    database_server.cpp
    persistentdb_server.cpp
)
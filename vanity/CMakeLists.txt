cmake_minimum_required(VERSION 3.22)
project(vanity)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(vanity
        database.cpp
        database_server.cpp
        instruction_server.cpp
        persistentdb_server.cpp
        socket/socket.cpp
        socket/socket_event_handler.cpp
        socket/socket_connection_server.cpp
        socket/socket_reader.cpp
        socket/socket_server.cpp
        socket/socket_writer.cpp
        vanity.cpp
)

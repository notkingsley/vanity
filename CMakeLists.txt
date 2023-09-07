cmake_minimum_required(VERSION 3.26)
project(vanity)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(vanity
        echo.cpp
        socket.cpp
        socket_reader.cpp
        socket_client.cpp
        socket_event_handler.cpp
)

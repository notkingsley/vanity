cmake_minimum_required(VERSION 3.22)
project(vanity)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_subdirectory(db)
add_subdirectory(socket)

add_executable(vanity
	auth_server.cpp
	request_server.cpp
	session_server.cpp
    vanity.cpp
)

target_link_libraries(vanity
    vanity_db
    vanity_socket
)

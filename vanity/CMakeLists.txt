cmake_minimum_required(VERSION 3.22)
project(vanity)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_subdirectory(auth)
add_subdirectory(db)
add_subdirectory(socket)

add_executable(vanity
	auth/auth_server.cpp
	client.cpp
	request_server.cpp
	response.cpp
	serialize.cpp
	session_server.cpp
    vanity.cpp
)

target_link_libraries(vanity
	vanity_auth
    vanity_db
    vanity_socket
)

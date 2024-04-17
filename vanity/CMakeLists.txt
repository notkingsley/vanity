cmake_minimum_required(VERSION 3.22)
project(vanity)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_subdirectory(auth)
add_subdirectory(client)
add_subdirectory(db)
add_subdirectory(request)
add_subdirectory(response)
add_subdirectory(socket)
add_subdirectory(wal)

add_library(libvanity STATIC
	bind_server.cpp
	cluster_server.cpp
	config.cpp
	event_server.cpp
	pipe_server.cpp
	poll_server.cpp
	pubsub_server.cpp
	repeat_event_server.cpp
	server.cpp
	session_server.cpp
	transaction_server.cpp
)

target_link_libraries(libvanity
	vanity_auth
	vanity_client
    vanity_db
	vanity_request
	vanity_response
    vanity_socket
	vanity_wal
)

add_executable(vanity
	vanity.cpp
)

target_link_libraries(vanity
	libvanity
)

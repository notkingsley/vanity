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

add_executable(vanity
	config.cpp
	event_server.cpp
	pipe_server.cpp
	pubsub_server.cpp
	session_server.cpp
	transaction_server.cpp
    vanity.cpp
)

target_link_libraries(vanity
	vanity_auth
	vanity_client
    vanity_db
	vanity_request
	vanity_response
    vanity_socket
)

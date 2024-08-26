project(vanity_request)

include_directories(.)

add_library(vanity_request STATIC
	extractable.cpp
	peer_request.cpp
	peer_request_server.cpp
	request.cpp
	request_handler.cpp
	request_server.cpp
)
project(vanity_request)

include_directories(.)

add_library(vanity_request STATIC
	request.cpp
	request_server.cpp
)
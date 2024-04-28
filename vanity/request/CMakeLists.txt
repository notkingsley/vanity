project(vanity_request)

include_directories(.)

add_library(vanity_request STATIC
	extractable.cpp
	request.cpp
	request_server.cpp
)
project(vanity_response)

include_directories(.)

add_library(vanity_response STATIC
	response.cpp
	sendable.cpp
)

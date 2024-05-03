project(vanity_response)

include_directories(.)

add_library(vanity_response STATIC
	post_message.cpp
	post_message_server.cpp
	response.cpp
	sendable.cpp
)

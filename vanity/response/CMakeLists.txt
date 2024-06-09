project(vanity_response)

include_directories(.)

add_library(vanity_response STATIC
	async_message.cpp
	peer_message_server.cpp
	post_message.cpp
	reply_message.cpp
	response.cpp
	sendable.cpp
)

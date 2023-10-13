project(vanity_auth)

include_directories(.)

add_library(vanity_auth STATIC
	auth_server.h
	auth_server.cpp
)
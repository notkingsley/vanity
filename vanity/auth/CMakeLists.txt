project(vanity_auth)

include_directories(.)

add_subdirectory(bcrypt)

add_library(vanity_auth STATIC
	auth_server.cpp
	locked_auth_server.cpp
	serial_auth_server.cpp
)

target_link_libraries(vanity_auth vanity_auth_bcrypt)
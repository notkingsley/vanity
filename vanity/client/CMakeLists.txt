project(vanity_client)

include_directories(.)

add_library(vanity_client STATIC
	tcp_client.cpp
	proxy_client.cpp
)

# cyclical dependency, see socket/CMakeLists.txt
target_link_libraries(vanity_client
	vanity_socket
)
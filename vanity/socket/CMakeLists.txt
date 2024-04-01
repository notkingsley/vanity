project(vanity_socket)

include_directories(.)

add_library(vanity_socket STATIC
    epoll.cpp
    socket.cpp
	socket_connection_server.cpp
    socket_reader.cpp
    socket_server.cpp
    socket_writer.cpp
)

# cyclic dependency see client/CMakeLists.txt
target_link_libraries(vanity_socket
	vanity_client
)
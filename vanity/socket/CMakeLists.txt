project(vanity_socket)

include_directories(.)

add_library(vanity_socket STATIC
    epoll.cpp
    socket.cpp
    socket_event_handler.cpp
    socket_connection_server.cpp
    socket_reader.cpp
    socket_server.cpp
    socket_writer.cpp
)
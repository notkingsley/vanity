project(vanity_cluster)

include_directories(.)

add_library(vanity_cluster STATIC
	cluster_auth_server.cpp
	cluster_server.cpp
	peer_auth_application_server.cpp
	peer_behaviour_server.cpp
	peer_server.cpp
)

project(vanity_cluster)

include_directories(.)

add_library(vanity_cluster STATIC
	address_server.cpp
	cluster_auth_server.cpp
	cluster_server.cpp
	high_peer_server.cpp
	peer_auth_application_server.cpp
	peer_behaviour_server.cpp
	peer_pulse_server.cpp
	peer_server.cpp
	peer_eviction_server.cpp
	peer_eviction_server.h
)

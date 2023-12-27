project(vanity_auth_bcrypt)

include_directories(.)

add_library(vanity_auth_bcrypt STATIC
	bcrypt.c
	bcrypt.h
	crypt.h
	crypt_blowfish.c
	crypt_blowfish.h
	crypt_gensalt.c
	crypt_gensalt.h
	ow-crypt.h
	wrapper.c
	x86.S
)
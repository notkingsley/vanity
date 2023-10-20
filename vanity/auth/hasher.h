//
// Created by kingsli on 10/13/23.
//

#ifndef VANITY_HASHER_H
#define VANITY_HASHER_H

#include <string>

#include "auth/bcrypt/bcrypt.h"
#include "exceptions.h"


namespace vanity {

/*
 * A stateless hasher that can hash a string,
 * and check if a string matches a hash.
 */
class Hasher {
public:
	Hasher() = default;
	~Hasher() = default;

	// work factor for the hash
	// between 4 and 31, inclusive
	// higher is more secure, but slower
	static constexpr int M_WORK_FACTOR = 4;

	// returns a hash of the string
	static std::string make_hash(const std::string& str) {
		char salt[BCRYPT_HASHSIZE], hash[BCRYPT_HASHSIZE];

		if (bcrypt_gensalt(M_WORK_FACTOR, salt))
			throw HashError("Could not generate salt");

		if (bcrypt_hashpw(str.c_str(), salt, hash))
			throw HashError("Could not hash password");

		return hash;
	};

	// returns true if the string matches the hash
	static bool check_hash(const std::string& str, const std::string& hash) {
		char new_hash[BCRYPT_HASHSIZE];
		if (bcrypt_hashpw(str.c_str(), hash.c_str(), new_hash))
			throw HashError("Could not check password hash");

		return hash == new_hash;
	}
};

} // namespace vanity

#endif //VANITY_HASHER_H

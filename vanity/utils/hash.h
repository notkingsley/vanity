//
// Created by kingsli on 4/17/24.
//

#ifndef VANITY_HASH_H
#define VANITY_HASH_H

#include <climits>
#include <cstdint>
#include <functional>
#include <utility>


namespace vanity::boost_hash {

/*
 * This is a reimplementation of the boost::hash_combine and boost::hash_value functions
 * from the boost library. We use these functions here to hash std::pair objects.
 *
 * They are copied here verbatim from the boost library, to avoid
 * depending on the boost library.
 *
 * The original implementations can be found at:
 *
 * https://github.com/boostorg/container_hash/blob/master/include/boost/container_hash/detail/hash_mix.hpp
 * for hash_mix and hash_mix_impl
 *
 * https://github.com/boostorg/container_hash/blob/master/include/boost/container_hash/hash.hpp#L471
 * for hash_combine
 *
 * https://github.com/boostorg/container_hash/blob/master/include/boost/container_hash/hash.hpp#L233
 * for hash_value
 *
 *
 * Finally, the boost_hash::hash class is provided as a drop-in replacement for std::hash
 */

template<std::size_t Bits>
struct hash_mix_impl;

template<>
struct hash_mix_impl<64> {
	inline static std::uint64_t fn(std::uint64_t x) {
		std::uint64_t const m = 0xe9846af9b1a615d;

		x ^= x >> 32;
		x *= m;
		x ^= x >> 32;
		x *= m;
		x ^= x >> 28;

		return x;
	}
};

template<>
struct hash_mix_impl<32> {
	inline static std::uint32_t fn(std::uint32_t x) {
		std::uint32_t const m1 = 0x21f0aaad;
		std::uint32_t const m2 = 0x735a2d97;

		x ^= x >> 16;
		x *= m1;
		x ^= x >> 15;
		x *= m2;
		x ^= x >> 15;

		return x;
	}
};

inline std::size_t hash_mix(std::size_t v) {
	return hash_mix_impl<sizeof(std::size_t) * CHAR_BIT>::fn(v);
}

template<class T>
inline void hash_combine(std::size_t &seed, T const &v) {
	seed = hash_mix(seed + 0x9e3779b9 + std::hash<T>()(v));
}

template<class A, class B>
std::size_t hash_value(std::pair<A, B> const &v) {
	std::size_t seed = 0;

	hash_combine(seed, v.first);
	hash_combine(seed, v.second);

	return seed;
}

template<class A, class B>
class hash {
public:
	std::size_t operator()(std::pair<A, B> const &v) const {
		return hash_value(v);
	}
};

} // namespace vanity::boost_hash

#endif //VANITY_HASH_H

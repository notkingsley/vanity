//
// Created by kingsli on 6/12/24.
//

#ifndef VANITY_RANDOM_H
#define VANITY_RANDOM_H

#include <string>
#include <random>

namespace vanity {

std::string random_string(size_t length, const char *prefix = "") {
	static auto& chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	thread_local static std::mt19937 rg{std::random_device{}()};
	thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chars) - 2);

	std::string s{prefix};
	s.reserve(s.size() + length);

	while (length--)
		s += chars[pick(rg)];

	return s;
}

} // namespace vanity

#endif //VANITY_RANDOM_H

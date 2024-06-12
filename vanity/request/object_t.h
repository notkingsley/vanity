//
// Created by kingsli on 4/20/24.
//

#ifndef VANITY_OBJECT_T_H
#define VANITY_OBJECT_T_H

#include <list>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "client/session_info.h"


namespace vanity {

// all object types
enum class object_t{
	STR,
	INT,
	FLOAT,
	ARR,
	LIST,
	SET,
	HASH,
	BOOL,
	CLIENT_AUTH,
};

const std::initializer_list<std::pair<object_t, std::string>> OBJECT_T_STRINGS {
	{object_t::STR,   "STR"},
	{object_t::INT,   "INT"},
	{object_t::FLOAT, "FLOAT"},
	{object_t::ARR,   "ARR"},
	{object_t::LIST,  "LIST"},
	{object_t::SET,   "SET"},
	{object_t::HASH,  "HASH"},
	{object_t::BOOL,  "BOOL"},
	{object_t::CLIENT_AUTH, "CLIENT_AUTH"},
};

template<object_t _obj>
struct concrete_traits;

template<>
struct concrete_traits<object_t::STR> {
	using type = std::string;
};

template<>
struct concrete_traits<object_t::INT> {
	using type = int64_t;
};

template<>
struct concrete_traits<object_t::FLOAT> {
	using type = double;
};

template<>
struct concrete_traits<object_t::ARR> {
	using type = std::vector<std::string>;
};

template<>
struct concrete_traits<object_t::LIST> {
	using type = std::list<std::string>;
};

template<>
struct concrete_traits<object_t::SET> {
	using type = std::unordered_set<std::string>;
};

template<>
struct concrete_traits<object_t::HASH> {
	using type = std::unordered_map<std::string, std::string>;
};

template<>
struct concrete_traits<object_t::BOOL> {
	using type = bool;
};

template<>
struct concrete_traits<object_t::CLIENT_AUTH> {
	using type = client_auth;
};

template <object_t ...Args>
struct concrete {
	using type = std::tuple<typename concrete_traits<Args>::type...>;
};

template<object_t _obj>
struct concrete<_obj> {
	using type = typename concrete_traits<_obj>::type;
};

template<object_t ...Args>
using concrete_t = typename concrete<Args...>::type;



} // namespace vanity

#endif //VANITY_OBJECT_T_H

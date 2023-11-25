//
// Created by kingsli on 11/25/23.
//

#ifndef VANITY_TYPES_H
#define VANITY_TYPES_H

#include <exception>
#include <fstream>
#include <list>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <unordered_set>


namespace vanity::db {

using string_t = std::string;
using int_t = int64_t;
using float_t = double;

using list_t = std::list<string_t>;
using set_t = std::unordered_set<string_t>;
using hash_t = std::unordered_map<string_t, string_t>;

using db_key_type = string_t;

using db_data_type = std::variant<string_t, int_t, float_t, list_t, set_t, hash_t>;

using db_pair_type = std::pair<db_key_type , db_data_type>;

template<size_t I>
using db_index_t = typename std::variant_alternative_t<I, db_data_type>;

} // namespace vanity::db

#endif //VANITY_TYPES_H

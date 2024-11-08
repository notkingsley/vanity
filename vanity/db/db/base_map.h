//
// Created by kingsli on 11/25/23.
//

#ifndef VANITY_BASE_MAP_H
#define VANITY_BASE_MAP_H

#include "types.h"

namespace vanity::db {

/*
 * A BaseMap simply contains a map of keys to values
 */
class BaseMap
{
protected:
	using key_type = db_key_type;
	using data_type = db_data_type;

	// the key value store
	std::unordered_map<key_type, data_type> m_data;
};

} // namespace vanity::db

#endif //VANITY_BASE_MAP_H

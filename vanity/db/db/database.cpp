//
// Created by kingsli on 9/24/23.
//

#include "database.h"
#include "utils/serializer.h"

namespace vanity::db {

Database &Database::operator=(Database &&other) noexcept
{
	BaseDatabase::operator=(std::move(other));
	return *this;
}

Database::Database(Database &&other) noexcept
	: BaseDatabase(std::move(other)) { }

void Database::persist(std::ofstream &out) {
	serializer::write(out, m_data);
	serializer::write(out, m_expiry_times);
}

Database Database::from(std::ifstream &in) {
	Database db;

	auto size = serializer::read<size_t>(in);
	for (size_t i = 0; i < size; ++i)
		db.m_data.insert(serializer::read<db_pair_type>(in));

	size = serializer::read<size_t>(in);
	for (size_t i = 0; i < size; ++i)
		db.m_expiry_times.insert(serializer::read<expiry_db_pair_type>(in));

	return db;
}

} // namespace vanity::db
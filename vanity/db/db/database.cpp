//
// Created by kingsli on 9/24/23.
//

#include "database.h"
#include "utils/serializer.h"

namespace vanity::db {

Database::Database(m_index_type index) : m_index{index} {}

Database &Database::operator=(Database &&other) noexcept {
	BaseDatabase::operator=(std::move(other));
	return *this;
}

Database::Database(Database &&other) noexcept
	: BaseDatabase(std::move(other)) {}

void Database::persist(std::ofstream &out) {
	serializer::write(out, m_index);
	serializer::write(out, m_data);
	serializer::write(out, m_expiry_times);
}

Database Database::from(std::ifstream &in) {
	auto index = serializer::read<m_index_type>(in);
	Database db {index};

	auto size = serializer::read<size_t>(in);
	for (size_t i = 0; i < size; ++i)
		db.m_data.insert(serializer::read<db_key_type, db_data_type>(in));

	size = serializer::read<size_t>(in);
	for (size_t i = 0; i < size; ++i)
		db.m_expiry_times.insert(serializer::read<db_key_type, time_t>(in));

	return db;
}

} // namespace vanity::db
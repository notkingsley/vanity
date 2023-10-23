//
// Created by kingsli on 10/20/23.
//

#include "serial_database.h"

namespace vanity::db {

SerialDatabase::SerialDatabase(Database &&other) noexcept: Database{std::move(other)} {}

SerialDatabase &SerialDatabase::operator=(Database &&other) noexcept {
	Database::operator=(std::move(other));
	return *this;
}

void SerialDatabase::perform(task_type task, data_type data, std::promise<ret_type> promise) {
	switch (task) {
		case task_type::GET:
		{
			auto& key = std::get<get_type>(data);
			promise.set_value(Database::get(key));
			break;
		}
		case task_type::SET:
		{
			auto& [key, value] = std::get<set_type>(data);
			Database::set(key, value);
			promise.set_value(std::monostate{});
			break;
		}
		case task_type::DEL:
		{
			auto& key = std::get<del_type>(data);
			promise.set_value(Database::del(key));
			break;
		}
		case task_type::TYPE:
		{
			auto& key = std::get<type_type>(data);
			promise.set_value(Database::type(key));
			break;
		}
		case task_type::HAS:
		{
			auto& key = std::get<has_type>(data);
			promise.set_value(Database::has(key));
			break;
		}
		case task_type::INCR_INT:
		{
			auto& [key, value] = std::get<incr_int_type>(data);
			promise.set_value(Database::incr_int(key, value));
			break;
		}
		case task_type::INCR_FLOAT:
		{
			auto& [key, value] = std::get<incr_float_type>(data);
			promise.set_value(Database::incr_float(key, value));
			break;
		}
		case task_type::RESET:
		{
			Database::reset();
			promise.set_value(std::monostate{});
			break;
		}
		case task_type::PERSIST:
		{
			auto& [out] = std::get<persist_type>(data);
			Database::persist(out);
			promise.set_value(std::monostate{});
			break;
		}
	}
}

SerialDatabase SerialDatabase::from(std::ifstream &in) {
	return SerialDatabase{Database::from(in)};
}

void SerialDatabase::start(long max_timeout) {
	TaskSerializer::start(max_timeout);
}

void SerialDatabase::stop() {
	TaskSerializer::stop();
}

void SerialDatabase::persist(std::ofstream &out) {
	send_task(task_type::PERSIST, out).wait();
}

void SerialDatabase::reset() {
	send_task(task_type::RESET, std::monostate{}).wait();
}

bool SerialDatabase::has(const Database::key_type &key) {
	return std::get<bool>(send_task(task_type::HAS, key).get());
}

std::optional<Database::data_type> SerialDatabase::get(const Database::key_type &key) {
	return std::get<std::optional<Database::data_type>>(send_task(task_type::GET, key).get());
}

void SerialDatabase::set(const Database::key_type &key, const Database::data_type &value) {
	send_task(task_type::SET, std::make_tuple(key, value)).wait();
}

bool SerialDatabase::del(const Database::key_type &key) {
	return std::get<bool>(send_task(task_type::DEL, key).get());
}

std::optional<int> SerialDatabase::type(const Database::key_type &key) {
	return std::get<std::optional<int>>(send_task(task_type::TYPE, key).get());
}

std::optional<int_t> SerialDatabase::incr_int(const Database::key_type &key, int_t value) {
	return std::get<std::optional<int_t>>(send_task(task_type::INCR_INT, std::make_tuple(key, value)).get());
}

std::optional<float_t> SerialDatabase::incr_float(const Database::key_type &key, float_t value) {
	return std::get<std::optional<float_t>>(send_task(task_type::INCR_FLOAT, std::make_tuple(key, value)).get());
}


} // namespace vanity::db
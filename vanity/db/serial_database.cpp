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

void SerialDatabase::perform(task_type task, task_data_type data, std::promise<ret_type> promise) {
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
		case task_type::LEN_STR:
		{
			auto& key = std::get<len_str_type>(data);
			promise.set_value(Database::len_str(key));
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
		case task_type::MANY_GET:
		{
			auto& keys = std::get<many_get_type>(data);
			promise.set_value(Database::many_get(keys));
			break;
		}
		case task_type::MANY_SET:
		{
			auto& keys = std::get<many_set_type>(data);
			Database::many_set(std::move(keys));
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
	return std::get<has_ret_type>(send_task(task_type::HAS, key).get());
}

std::optional<Database::data_type> SerialDatabase::get(const Database::key_type &key) {
	return std::get<get_ret_type>(send_task(task_type::GET, key).get());
}

void SerialDatabase::set(const Database::key_type &key, const Database::data_type &value) {
	send_task(task_type::SET, std::make_tuple(key, value)).wait();
}

bool SerialDatabase::del(const Database::key_type &key) {
	return std::get<del_ret_type>(send_task(task_type::DEL, key).get());
}

std::optional<int> SerialDatabase::type(const Database::key_type &key) {
	return std::get<type_ret_type>(send_task(task_type::TYPE, key).get());
}

std::optional<int_t> SerialDatabase::incr_int(const Database::key_type &key, int_t value) {
	return std::get<incr_int_ret_type>(send_task(task_type::INCR_INT, std::make_tuple(key, value)).get());
}

std::optional<float_t> SerialDatabase::incr_float(const Database::key_type &key, float_t value) {
	return std::get<incr_float_ret_type>(send_task(task_type::INCR_FLOAT, std::make_tuple(key, value)).get());
}

std::optional<int_t> SerialDatabase::len_str(const Database::key_type &key) {
	return std::get<len_str_ret_type>(send_task(task_type::LEN_STR, key).get());
}

std::vector<std::optional<Database::data_type>> SerialDatabase::many_get(const std::vector<key_type> &keys) {
	return std::get<many_get_ret_type>(send_task(task_type::MANY_GET, keys).get());
}

void SerialDatabase::many_set(std::vector<std::pair<db_key_type, db_data_type>> keys) {
	send_task(task_type::MANY_SET, std::move(keys)).wait();
}

} // namespace vanity::db
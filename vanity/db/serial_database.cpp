//
// Created by kingsli on 10/20/23.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#include "serial_database.h"

namespace vanity::db {

SerialDatabase::SerialDatabase(Database &&other) noexcept: Database{std::move(other)} {}

SerialDatabase &SerialDatabase::operator=(Database &&other) noexcept {
	Database::operator=(std::move(other));
	return *this;
}

void SerialDatabase::perform(task_type task, task_data_type data, std::promise<ret_type> promise) {
	switch (task) {
		case GET:
		{
			auto& key = std::get<get_type>(data);
			promise.set_value(Database::get(key));
			break;
		}
		case SET:
		{
			auto& [key, value] = std::get<set_type>(data);
			Database::set(key, value);
			promise.set_value(std::monostate{});
			break;
		}
		case DEL:
		{
			auto& key = std::get<del_type>(data);
			promise.set_value(Database::del(key));
			break;
		}
		case TYPE:
		{
			auto& key = std::get<type_type>(data);
			promise.set_value(Database::type(key));
			break;
		}
		case HAS:
		{
			auto& key = std::get<has_type>(data);
			promise.set_value(Database::has(key));
			break;
		}
		case INCR_INT:
		{
			auto& [key, value] = std::get<incr_int_type>(data);
			promise.set_value(Database::incr_int(key, value));
			break;
		}
		case INCR_FLOAT:
		{
			auto& [key, value] = std::get<incr_float_type>(data);
			promise.set_value(Database::incr_float(key, value));
			break;
		}
		case STR_LEN:
		{
			auto& key = std::get<str_len_type>(data);
			promise.set_value(Database::str_len(key));
			break;
		}
		case RESET:
		{
			Database::reset();
			promise.set_value(std::monostate{});
			break;
		}
		case PERSIST:
		{
			auto& [out] = std::get<persist_type>(data);
			Database::persist(out);
			promise.set_value(std::monostate{});
			break;
		}
		case MANY_GET:
		{
			auto& keys = std::get<many_get_type>(data);
			promise.set_value(Database::many_get(keys));
			break;
		}
		case MANY_SET:
		{
			auto& keys = std::get<many_set_type>(data);
			Database::many_set(std::move(keys));
			promise.set_value(std::monostate{});
			break;
		}
		case LIST_LEN:
		{
			auto& key = std::get<list_len_type>(data);
			promise.set_value(Database::list_len(key));
			break;
		}
		case LIST_GET:
		{
			auto& [key, index] = std::get<list_get_type>(data);
			promise.set_value(Database::list_get(key, index));
			break;
		}
		case LIST_SET:
		{
			auto& [key, index, value] = std::get<list_set_type>(data);
			promise.set_value(Database::list_set(key, index, value));
			break;
		}
		case LIST_PUSH_LEFT:
		{
			auto& [key, values] = std::get<list_push_left_type>(data);
			promise.set_value(Database::list_push_left(key, std::move(values)));
			break;
		}
		case LIST_PUSH_RIGHT:
		{
			auto& [key, values] = std::get<list_push_right_type>(data);
			promise.set_value(Database::list_push_right(key, std::move(values)));
			break;
		}
		case LIST_POP_LEFT:
		{
			auto& [key, n] = std::get<list_pop_left_type>(data);
			promise.set_value(Database::list_pop_left(key, n));
			break;
		}
		case LIST_POP_RIGHT:
		{
			auto& [key, n] = std::get<list_pop_right_type>(data);
			promise.set_value(Database::list_pop_right(key, n));
			break;
		}
		case LIST_RANGE:
		{
			auto& [key, start, stop] = std::get<list_range_type>(data);
			promise.set_value(Database::list_range(key, start, stop));
			break;
		}
		case LIST_TRIM:
		{
			auto& [key, start, stop] = std::get<list_trim_type>(data);
			promise.set_value(Database::list_trim(key, start, stop));
			break;
		}
		case LIST_REMOVE:
		{
			auto& [key, value, count] = std::get<list_remove_type>(data);
			promise.set_value(Database::list_remove(key, value, count));
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
	send_task(PERSIST, out).wait();
}

void SerialDatabase::reset() {
	send_task(RESET, std::monostate{}).wait();
}

bool SerialDatabase::has(const key_type &key) {
	return std::get<has_ret_type>(send_task(HAS, key).get());
}

bool SerialDatabase::del(const key_type &key) {
	return std::get<del_ret_type>(send_task(DEL, key).get());
}


auto SerialDatabase::get(const key_type &key) -> get_ret_type {
	return std::get<get_ret_type>(send_task(GET, key).get());
}

void SerialDatabase::set(const key_type &key, const data_type& value) {
	send_task(SET, std::make_tuple(key, value)).wait();
}

auto SerialDatabase::type(const key_type &key) -> type_ret_type {
	return std::get<type_ret_type>(send_task(TYPE, key).get());
}

auto SerialDatabase::incr_int(const key_type &key, int_t value) -> incr_int_ret_type{
	return std::get<incr_int_ret_type>(send_task(INCR_INT, std::make_tuple(key, value)).get());
}

auto SerialDatabase::incr_float(const key_type &key, float_t value) -> incr_float_ret_type {
	return std::get<incr_float_ret_type>(send_task(INCR_FLOAT, std::make_tuple(key, value)).get());
}

auto SerialDatabase::str_len(const key_type &key) -> str_len_ret_type {
	return std::get<str_len_ret_type>(send_task(STR_LEN, key).get());
}

auto SerialDatabase::many_get(const std::vector<key_type> &keys) -> many_get_ret_type {
	return std::move(std::move(std::get<many_get_ret_type>(send_task(MANY_GET, keys).get())));
}

void SerialDatabase::many_set(std::vector<std::pair<db_key_type, db_data_type>> keys) {
	send_task(MANY_SET, std::move(keys)).wait();
}


auto SerialDatabase::list_len(const key_type &key) -> list_len_ret_type{
	return std::get<list_len_ret_type>(send_task(LIST_LEN, key).get());
}

auto SerialDatabase::list_get(const key_type &key, int index) -> list_get_ret_type {
	return std::get<list_get_ret_type>(send_task(LIST_GET, std::make_tuple(key, index)).get());
}

auto SerialDatabase::list_set(const key_type &key, int index, const std::string &value) -> list_set_ret_type {
	return std::get<list_set_ret_type>(send_task(LIST_SET, std::make_tuple(key, index, value)).get());
}

auto SerialDatabase::list_push_left(const key_type &key, list_t values) -> list_push_left_ret_type {
	return std::get<list_push_left_ret_type>(send_task(LIST_PUSH_LEFT, std::make_tuple(key, std::move(values))).get());
}

auto SerialDatabase::list_push_right(const key_type &key, list_t values) -> list_push_right_ret_type {
	return std::get<list_push_right_ret_type>(send_task(LIST_PUSH_RIGHT, std::make_tuple(key, std::move(values))).get());
}

auto SerialDatabase::list_pop_left(const key_type &key, int n) -> list_pop_left_ret_type {
	return std::get<list_pop_left_ret_type>(send_task(LIST_POP_LEFT, std::make_tuple(key, n)).get());
}

auto SerialDatabase::list_pop_right(const key_type &key, int n) -> list_pop_right_ret_type {
	return std::get<list_pop_right_ret_type>(send_task(LIST_POP_RIGHT, std::make_tuple(key, n)).get());
}

auto SerialDatabase::list_range(const key_type &key, int start, int stop) -> list_range_ret_type {
	return std::get<list_range_ret_type>(send_task(LIST_RANGE, std::make_tuple(key, start, stop)).get());
}

auto SerialDatabase::list_trim(const key_type &key, int start, int stop) -> list_trim_ret_type {
	return std::get<list_trim_ret_type>(send_task(LIST_TRIM, std::make_tuple(key, start, stop)).get());
}

auto SerialDatabase::list_remove(const key_type &key, const std::string &value, int count) -> list_remove_ret_type {
	return std::get<list_remove_ret_type>(send_task(LIST_REMOVE, std::make_tuple(key, value, count)).get());
}

} // namespace vanity::db

#pragma clang diagnostic pop
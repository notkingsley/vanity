import unittest

from client.server_handle import ServerHandle
from tests.utils import get_free_port, make_client


class BaseDatabaseTest(unittest.TestCase):
	@classmethod
	def setUpClass(cls) -> None:
		port = get_free_port()
		cls.server_handle = ServerHandle(port= port)
		cls.server_handle.start()
		cls.client = make_client(port)
	
	@classmethod
	def tearDownClass(cls) -> None:
		cls.client.close()
		cls.server_handle.stop()
	
	def tearDown(self):
		self.client.reset()


class GeneralDatabaseTest(BaseDatabaseTest):
	def test_get_empty(self):
		"""
		Test that we get a null when we try to get a key that doesn't exist.
		"""
		response = self.client.get("test_get_empty")
		self.assertTrue(response.is_null())
	
	def test_del_empty(self):
		"""
		Test that we get an error when we try to delete a key that doesn't exist.
		"""
		response = self.client.delete("test_del_empty")
		self.assertTrue(response.is_error())
	
	def test_ping(self):
		"""
		Test that we can ping the server.
		"""
		response = self.client.ping()
		self.assertTrue(response.is_pong())
	
	def test_reset(self):
		"""
		Test that we can reset the database.
		"""
		self.client.str_set("test_reset", "test_reset_value")
		self.client.reset()
		response = self.client.get("test_reset")
		self.assertTrue(response.is_null())

	def test_get_type_str(self):
		"""
		Test that the type of a string is str.
		"""
		self.client.str_set("test_type_str", "test_type_str_value")
		response = self.client.get("test_type_str")
		self.assertTrue(response.type_is_str())
	
	def test_get_type_int(self):
		"""
		Test that the type of an integer is int.
		"""
		self.client.int_set("test_type_int", 123)
		response = self.client.get("test_type_int")
		self.assertTrue(response.type_is_int())
	
	def test_get_type_float(self):
		"""
		Test that the type of a float is float.
		"""
		self.client.float_set("test_type_float", 123.456)
		response = self.client.get("test_type_float")
		self.assertTrue(response.type_is_float())
	
	def test_get_type_null(self):
		"""
		Test that the type of a null is None.
		"""
		response = self.client.get("test_type_null")
		self.assertEqual(response.type, None)
	
	def test_type_str(self):
		"""
		Test that we can check if a value is a string.
		"""
		self.client.str_set("test_type_str", "test_type_str_value")
		response = self.client.type("test_type_str")
		self.assertTrue(response.type_is_str())
		self.assertIsNone(response.value)
	
	def test_type_int(self):
		"""
		Test that we can check if a value is an integer.
		"""
		self.client.int_set("test_type_int", 123)
		response = self.client.type("test_type_int")
		self.assertTrue(response.type_is_int())
		self.assertIsNone(response.value)
	
	def test_type_float(self):
		"""
		Test that we can check if a value is a float.
		"""
		self.client.float_set("test_type_float", 123.456)
		response = self.client.type("test_type_float")
		self.assertTrue(response.type_is_float())
		self.assertIsNone(response.value)
	
	def test_exists(self):
		"""
		Test that we can check if a key exists.
		"""
		self.client.str_set("test_exists", "test_exists_value")
		response = self.client.exists("test_exists")
		self.assertTrue(response.is_ok())
	
	def test_exists_not(self):
		"""
		Test that we can check if a key doesn't exist.
		"""
		response = self.client.exists("test_exists_not")
		self.assertTrue(response.is_null())
	
	def test_exists_delete(self):
		"""
		Test that we can check if a key exists after deleting it.
		"""
		response = self.client.str_set("test_exists_delete", "test_exists_delete_value")
		self.assertTrue(response.is_ok())
		response = self.client.delete("test_exists_delete")
		self.assertTrue(response.is_ok())
		response = self.client.exists("test_exists_delete")
		self.assertTrue(response.is_null())
	
	def test_keys(self):
		"""
		Test that we can get the keys.
		"""
		self.client.str_set("test_keys_1", "test_keys_1_value")
		self.client.str_set("test_keys_2", "test_keys_2_value")
		self.client.str_set("test_keys_3", "test_keys_3_value")
		response = self.client.keys()
		self.assertTrue(response.is_ok())
		self.assertEqual(set(response.value), {"test_keys_1", "test_keys_2", "test_keys_3"})
	
	def test_keys_empty(self):
		"""
		Test that we can get the keys when there are none.
		"""
		response = self.client.keys()
		self.assertTrue(response.is_null())
	
	def test_copy_to(self):
		"""
		Test that we can copy a key to another key.
		"""
		self.client.str_set("test_copy_to", "test_copy_to_value")
		response = self.client.copy_to("test_copy_to", "test_copy_to_2")
		self.assertTrue(response.is_ok())
		response = self.client.get("test_copy_to_2")
		self.assertEqual(response.value, "test_copy_to_value")
	
	def test_copy_to_overwrite(self):
		"""
		Test that we can copy a key to another key, overwriting it.
		"""
		self.client.str_set("test_copy_to_overwrite", "test_copy_to_overwrite_value")
		self.client.str_set("test_copy_to_overwrite_2", "test_copy_to_overwrite_value_2")
		response = self.client.copy_to("test_copy_to_overwrite", "test_copy_to_overwrite_2")
		self.assertTrue(response.is_ok())
		response = self.client.get("test_copy_to_overwrite_2")
		self.assertEqual(response.value, "test_copy_to_overwrite_value")
	
	def test_copy_to_non_existent(self):
		"""
		Test that we can't copy a key to another key when the source key doesn't exist.
		"""
		response = self.client.copy_to("test_copy_to_non_existent", "test_copy_to_non_existent_2")
		self.assertTrue(response.is_null())
	
	def test_copy_to_same(self):
		"""
		Test that we can copy a key to itself and nothing happens.
		"""
		self.client.str_set("test_copy_to_same", "test_copy_to_same_value")
		response = self.client.copy_to("test_copy_to_same", "test_copy_to_same")
		self.assertTrue(response.is_ok())
		response = self.client.get("test_copy_to_same")
		self.assertEqual(response.value, "test_copy_to_same_value")

	def move_to(self):
		"""
		Test that we can move a key to another key.
		"""
		self.client.str_set("test_move_to", "test_move_to_value")
		response = self.client.move_to("test_move_to", "test_move_to_2")
		self.assertTrue(response.is_ok())
		response = self.client.get("test_move_to_2")
		self.assertEqual(response.value, "test_move_to_value")
		response = self.client.get("test_move_to")
		self.assertTrue(response.is_null())
	
	def test_move_to_overwrite(self):
		"""
		Test that we can move a key to another key, overwriting it.
		"""
		self.client.str_set("test_move_to_overwrite", "test_move_to_overwrite_value")
		self.client.str_set("test_move_to_overwrite_2", "test_move_to_overwrite_value_2")
		response = self.client.move_to("test_move_to_overwrite", "test_move_to_overwrite_2")
		self.assertTrue(response.is_ok())
		response = self.client.get("test_move_to_overwrite_2")
		self.assertEqual(response.value, "test_move_to_overwrite_value")
		response = self.client.get("test_move_to_overwrite")
		self.assertTrue(response.is_null())
	
	def test_move_to_non_existent(self):
		"""
		Test that we can't move a key to another key when the source key doesn't exist.
		"""
		response = self.client.move_to("test_move_to_non_existent", "test_move_to_non_existent_2")
		self.assertTrue(response.is_null())
	
	def test_move_to_same(self):
		"""
		Test that we can move a key to itself and nothing happens.
		"""
		self.client.str_set("test_move_to_same", "test_move_to_same_value")
		response = self.client.move_to("test_move_to_same", "test_move_to_same")
		self.assertTrue(response.is_ok())
		response = self.client.get("test_move_to_same")
		self.assertEqual(response.value, "test_move_to_same_value")
	
	def test_copy_to_db_non_existent(self):
		"""
		Test that we can't copy a key to another database when the source key doesn't exist.
		"""
		response = self.client.copy_to_db("test_copy_to_db_non_existent", 1)
		self.assertTrue(response.is_null())
	
	def test_copy_to_db_same(self):
		"""
		Test that we can copy a key to the same database and nothing happens.
		"""
		self.client.str_set("test_copy_to_db_same", "test_copy_to_db_same_value")
		response = self.client.copy_to_db("test_copy_to_db_same", 0)
		self.assertTrue(response.is_ok())
		response = self.client.get("test_copy_to_db_same")
		self.assertEqual(response.value, "test_copy_to_db_same_value")
	
	def test_move_to_db_non_existent(self):
		"""
		Test that we can't move a key to another database when the source key doesn't exist.
		"""
		response = self.client.move_to_db("test_move_to_db_non_existent", 1)
		self.assertTrue(response.is_null())
	
	def test_move_to_db_same(self):
		"""
		Test that we can move a key to the same database and nothing happens.
		"""
		self.client.str_set("test_move_to_db_same", "test_move_to_db_same_value")
		response = self.client.move_to_db("test_move_to_db_same", 0)
		self.assertTrue(response.is_ok())
		response = self.client.get("test_move_to_db_same")
		self.assertEqual(response.value, "test_move_to_db_same_value")


class GeneralDatabaseTestWithSwitching(BaseDatabaseTest):
	"""
	Test general database commands that modify multiple databases.
	"""
	def tearDown(self):
		self.client.switch_db(0)
		self.client.reset()
		self.client.switch_db(1)
		self.client.reset()
		self.client.switch_db(0)

	def test_copy_to_db(self):
		"""
		Test that we can copy a key to another database.
		"""
		self.client.str_set("test_copy_to_db", "test_copy_to_db_value")
		response = self.client.copy_to_db("test_copy_to_db", 1)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(1)
		self.assertTrue(response.is_ok())
		response = self.client.get("test_copy_to_db")
		self.assertEqual(response.value, "test_copy_to_db_value")
	
	def test_copy_to_db_overwrite(self):
		"""
		Test that we can copy a key to another database, overwriting it.
		"""
		self.client.str_set("test_copy_to_db_overwrite", "test_copy_to_db_overwrite_value")
		self.client.switch_db(1)
		self.client.str_set("test_copy_to_db_overwrite", "test_copy_to_db_overwrite_value_2")
		self.client.switch_db(0)
		response = self.client.copy_to_db("test_copy_to_db_overwrite", 1)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(1)
		response = self.client.get("test_copy_to_db_overwrite")
		self.assertEqual(response.value, "test_copy_to_db_overwrite_value")
	
	def test_move_to_db(self):
		"""
		Test that we can move a key to another database.
		"""
		self.client.str_set("test_move_to_db", "test_move_to_db_value")
		response = self.client.move_to_db("test_move_to_db", 1)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(1)
		self.assertTrue(response.is_ok())
		response = self.client.get("test_move_to_db")
		self.assertEqual(response.value, "test_move_to_db_value")
		response = self.client.switch_db(0)
		response = self.client.get("test_move_to_db")
		self.assertTrue(response.is_null())
	
	def test_move_to_db_overwrite(self):
		"""
		Test that we can move a key to another database, overwriting it.
		"""
		self.client.str_set("test_move_to_db_overwrite", "test_move_to_db_overwrite_value")
		self.client.switch_db(1)
		self.client.str_set("test_move_to_db_overwrite", "test_move_to_db_overwrite_value_2")
		self.client.switch_db(0)
		response = self.client.move_to_db("test_move_to_db_overwrite", 1)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(1)
		response = self.client.get("test_move_to_db_overwrite")
		self.assertEqual(response.value, "test_move_to_db_overwrite_value")
		response = self.client.switch_db(0)
		response = self.client.get("test_move_to_db_overwrite")
		self.assertTrue(response.is_null())


class PrimitiveDatabaseTest(BaseDatabaseTest):
	def test_set_get(self):
		"""
		Test that we can set a value and then get it.
		"""
		self.client.str_set("test_set_get_key", "test_set_get_value")
		response = self.client.get("test_set_get_key")
		self.assertEqual(response.value, "test_set_get_value")

	def test_set_del(self):
		"""
		Test that we can set a value and then delete it.
		"""
		self.client.str_set("test_set_del_key", "test_set_del_value")
		response = self.client.delete("test_set_del_key")
		self.assertTrue(response.is_ok())

	def test_set_overwrite(self):
		"""
		Test that we can overwrite a key.
		"""
		self.client.str_set("test_set_overwrite", "test_set_overwrite_value")
		self.client.str_set("test_set_overwrite", "test_set_overwrite_value2")
		response = self.client.get("test_set_overwrite")
		self.assertEqual(response.value, "test_set_overwrite_value2")
	
	def test_set_delete_get(self):
		"""
		Test that we can set a value, delete it, and then get a null.
		"""
		self.client.str_set("test_set_delete_get", "test_set_delete_get_value")
		self.client.delete("test_set_delete_get")
		response = self.client.get("test_set_delete_get")
		self.assertTrue(response.is_null())
	
	def test_set_delete_set_get(self):
		"""
		Test that we can set a value, delete it, set it again, and then get it.
		"""
		self.client.str_set("test_set_delete_set_get", "test_set_delete_set_get_value")
		self.client.delete("test_set_delete_set_get")
		self.client.str_set("test_set_delete_set_get", "test_set_delete_set_get_value2")
		response = self.client.get("test_set_delete_set_get")
		self.assertEqual(response.value, "test_set_delete_set_get_value2")
	
	def test_set_get_int(self):
		"""
		Test that we can set an integer value and then get it.
		"""
		self.client.int_set("test_set_get_int", 123)
		response = self.client.get("test_set_get_int")
		self.assertEqual(response.value, 123)
	
	def test_set_get_float(self):
		"""
		Test that we can set a float value and then get it.
		"""
		self.client.float_set("test_set_get_float", 123.456)
		response = self.client.get("test_set_get_float")
		self.assertEqual(response.value, 123.456)
	
	def test_incr_int(self):
		"""
		Test that we can increment an integer value.
		"""
		self.client.int_set("test_incr_int", 123)
		response = self.client.incr_int("test_incr_int", 456)
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, 579)
		response = self.client.get("test_incr_int")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, 579)
	
	def test_incr_int_non_existent(self):
		"""
		Test that we can increment a non-existent integer value.
		"""
		response = self.client.incr_int("test_incr_int_non_existent", 123)
		self.assertEqual(response.value, 123)
		self.assertTrue(response.is_ok())
		response = self.client.get("test_incr_int_non_existent")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, 123)
	
	def test_incr_float(self):
		"""
		Test that we can increment a float value.
		"""
		self.client.float_set("test_incr_float", 123.456)
		response = self.client.incr_float("test_incr_float", 456.789)
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, 580.245)
		response = self.client.get("test_incr_float")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, 580.245)
	
	def test_incr_float_non_existent(self):
		"""
		Test that we can increment a non-existent float value.
		"""
		response = self.client.incr_float("test_incr_float_non_existent", 123.456)
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, 123.456)
		response = self.client.get("test_incr_float_non_existent")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, 123.456)
	
	def test_decr_int(self):
		"""
		Test that we can decrement an integer value.
		"""
		self.client.int_set("test_decr_int", 123)
		response = self.client.decr_int("test_decr_int", 456)
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, -333)
		response = self.client.get("test_decr_int")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, -333)
	
	def test_decr_int_non_existent(self):
		"""
		Test that we can decrement a non-existent integer value.
		"""
		response = self.client.decr_int("test_decr_int_non_existent", 123)
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, -123)
		response = self.client.get("test_decr_int_non_existent")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, -123)
	
	def test_decr_float(self):
		"""
		Test that we can decrement a float value.
		"""
		self.client.float_set("test_decr_float", 123.456)
		response = self.client.decr_float("test_decr_float", 456.789)
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, -333.333)
		response = self.client.get("test_decr_float")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, -333.333)
	
	def test_decr_float_non_existent(self):
		"""
		Test that we can decrement a non-existent float value.
		"""
		response = self.client.decr_float("test_decr_float_non_existent", 123.456)
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, -123.456)
		response = self.client.get("test_decr_float_non_existent")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, -123.456)
	
	def test_incr_float_int(self):
		"""
		Test that we can increment a float value by an integer.
		"""
		self.client.float_set("test_incr_float_int", 123.456)
		response = self.client.incr_float("test_incr_float_int", 456)
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, 579.456)
		response = self.client.get("test_incr_float_int")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, 579.456)
	
	def test_incr_int_float(self):
		"""
		Test that we can't increment an integer value by a float.
		"""
		self.client.int_set("test_incr_int_float", 123)
		response = self.client.incr_float("test_incr_int_float", 456.789)
		self.assertTrue(response.is_bad_type())
		response = self.client.get("test_incr_int_float")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, 123)
	
	def test_str_len(self):
		"""
		Test that we can get the length of a string.
		"""
		self.client.str_set("test_str_len", "test_str_len_value")
		response = self.client.str_len("test_str_len")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, len("test_str_len_value"))
	
	def test_str_len_non_existent(self):
		"""
		Test that we can't get the length of a non-existent string.
		"""
		response = self.client.str_len("test_str_len_non_existent")
		self.assertTrue(response.is_bad_type())
	
	def test_str_len_int(self):
		"""
		Test that we can't get the length of an integer.
		"""
		self.client.int_set("test_str_len_int", 123)
		response = self.client.str_len("test_str_len_int")
		self.assertTrue(response.is_bad_type())
	
	def test_str_len_float(self):
		"""
		Test that we can't get the length of a float.
		"""
		self.client.float_set("test_str_len_float", 123.456)
		response = self.client.str_len("test_str_len_float")
		self.assertTrue(response.is_bad_type())
	
	def test_many_get(self):
		"""
		Test that we can get many values at once.
		"""
		self.client.str_set("test_many_get_1", "test_many_get_1_value")
		self.client.str_set("test_many_get_2", "test_many_get_2_value")
		self.client.str_set("test_many_get_3", "test_many_get_3_value")
		response = self.client.many_get("test_many_get_1", "test_many_get_2", "test_many_get_3")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, ["test_many_get_1_value", "test_many_get_2_value", "test_many_get_3_value"])
	
	def test_many_get_non_existent(self):
		"""
		Test that we can get many values at once, some of which don't exist.
		"""
		self.client.str_set("test_many_get_non_existent_1", "test_many_get_non_existent_1_value")
		self.client.str_set("test_many_get_non_existent_3", "test_many_get_non_existent_3_value")
		response = self.client.many_get("test_many_get_non_existent_1", "test_many_get_non_existent_2", "test_many_get_non_existent_3")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, ["test_many_get_non_existent_1_value", None, "test_many_get_non_existent_3_value"])
	
	def test_many_get_empty(self):
		"""
		Test that we can get many values at once, none of which exist.
		"""
		response = self.client.many_get("test_many_get_empty_1", "test_many_get_empty_2", "test_many_get_empty_3")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, [None, None, None])
	
	def test_many_get_int(self):
		"""
		Test that we can get many values at once, some of which are integers.
		"""
		self.client.str_set("test_many_get_int_1", "test_many_get_int_1_value")
		self.client.int_set("test_many_get_int_2", 123)
		self.client.str_set("test_many_get_int_3", "test_many_get_int_3_value")
		response = self.client.many_get("test_many_get_int_1", "test_many_get_int_2", "test_many_get_int_3")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, ["test_many_get_int_1_value", 123, "test_many_get_int_3_value"])
	
	def test_many_get_float(self):
		"""
		Test that we can get many values at once, some of which are floats.
		"""
		self.client.str_set("test_many_get_float_1", "test_many_get_float_1_value")
		self.client.float_set("test_many_get_float_2", 123.456)
		self.client.str_set("test_many_get_float_3", "test_many_get_float_3_value")
		response = self.client.many_get("test_many_get_float_1", "test_many_get_float_2", "test_many_get_float_3")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, ["test_many_get_float_1_value", 123.456, "test_many_get_float_3_value"])
	
	def test_many_get_mixed(self):
		"""
		Test that we can get many values at once, some of which are integers and floats.
		"""
		self.client.str_set("test_many_get_mixed_1", "test_many_get_mixed_1_value")
		self.client.int_set("test_many_get_mixed_2", 123)
		self.client.float_set("test_many_get_mixed_3", 123.456)
		self.client.str_set("test_many_get_mixed_4", "test_many_get_mixed_4_value")
		self.client.float_set("test_many_get_mixed_5", 123.456)
		response = self.client.many_get("test_many_get_mixed_1", "test_many_get_mixed_2", "test_invalid", "test_many_get_mixed_3", "test_many_get_mixed_4", "test_many_get_mixed_5")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, ["test_many_get_mixed_1_value", 123, None, 123.456, "test_many_get_mixed_4_value", 123.456])


class SwitchDBTest(unittest.TestCase):
	"""
	Test database index switching (incomplete)
	"""
	@classmethod
	def setUpClass(cls) -> None:
		cls.port = get_free_port()
		cls.server_handle = ServerHandle(port= cls.port)
		cls.server_handle.start()

	@classmethod
	def tearDownClass(cls) -> None:
		cls.server_handle.stop()

	def setUp(self) -> None:
		self.client = make_client(self.port)
	
	def tearDown(self) -> None:
		self.client.close()
	
	def test_switch_db(self):
		"""
		Test that we can switch databases.
		"""
		response = self.client.str_set("test_switch_db", "test_switch_db_value")
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(1)
		self.assertTrue(response.is_ok())
		response = self.client.get("test_switch_db")
		self.assertTrue(response.is_null())
		response = self.client.switch_db(0)
		self.assertTrue(response.is_ok())
		response = self.client.get("test_switch_db")
		self.assertEqual(response.value, "test_switch_db_value")
	
	def test_switch_db_fail(self):
		"""
		Test that we can't switch to a database that doesn't exist.
		"""
		response = self.client.switch_db(16)
		self.assertTrue(response.is_error())
		response = self.client.switch_db(-1)
		self.assertTrue(response.is_error())
	
	def test_switch_db_ok(self):
		"""
		Test that we can switch to a database that does exist.
		"""
		response = self.client.switch_db(0)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(1)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(2)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(3)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(4)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(5)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(6)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(7)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(8)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(9)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(10)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(11)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(12)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(13)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(14)
		self.assertTrue(response.is_ok())
		response = self.client.switch_db(15)
		self.assertTrue(response.is_ok())
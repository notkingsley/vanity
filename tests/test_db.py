import unittest

from client.server_handle import ServerHandle
from tests.utils import get_free_port, make_client


class DatabaseTest(unittest.TestCase):
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

	def test_set_get(self):
		"""
		Test that we can set a value and then get it.
		"""
		self.client.set("test_set_get_key", "test_set_get_value")
		response = self.client.get("test_set_get_key")
		self.assertEqual(response.value, "test_set_get_value")

	def test_set_del(self):
		"""
		Test that we can set a value and then delete it.
		"""
		self.client.set("test_set_del_key", "test_set_del_value")
		response = self.client.delete("test_set_del_key")
		self.assertTrue(response.is_ok())

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
	
	def test_set_overwrite(self):
		"""
		Test that we can overwrite a key.
		"""
		self.client.set("test_set_overwrite", "test_set_overwrite_value")
		self.client.set("test_set_overwrite", "test_set_overwrite_value2")
		response = self.client.get("test_set_overwrite")
		self.assertEqual(response.value, "test_set_overwrite_value2")
	
	def test_set_delete_get(self):
		"""
		Test that we can set a value, delete it, and then get a null.
		"""
		self.client.set("test_set_delete_get", "test_set_delete_get_value")
		self.client.delete("test_set_delete_get")
		response = self.client.get("test_set_delete_get")
		self.assertTrue(response.is_null())
	
	def test_set_delete_set_get(self):
		"""
		Test that we can set a value, delete it, set it again, and then get it.
		"""
		self.client.set("test_set_delete_set_get", "test_set_delete_set_get_value")
		self.client.delete("test_set_delete_set_get")
		self.client.set("test_set_delete_set_get", "test_set_delete_set_get_value2")
		response = self.client.get("test_set_delete_set_get")
		self.assertEqual(response.value, "test_set_delete_set_get_value2")
	
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
		self.client.set("test_reset", "test_reset_value")
		self.client.reset()
		response = self.client.get("test_reset")
		self.assertTrue(response.is_null())
	
	def test_set_get_int(self):
		"""
		Test that we can set an integer value and then get it.
		"""
		self.client.set("test_set_get_int", 123)
		response = self.client.get("test_set_get_int")
		self.assertEqual(response.value, 123)
	
	def test_set_get_float(self):
		"""
		Test that we can set a float value and then get it.
		"""
		self.client.set("test_set_get_float", 123.456)
		response = self.client.get("test_set_get_float")
		self.assertEqual(response.value, 123.456)
	
	def test_get_type_str(self):
		"""
		Test that the type of a string is str.
		"""
		self.client.set("test_type_str", "test_type_str_value")
		response = self.client.get("test_type_str")
		self.assertEqual(response.type, str)
	
	def test_get_type_int(self):
		"""
		Test that the type of an integer is int.
		"""
		self.client.set("test_type_int", 123)
		response = self.client.get("test_type_int")
		self.assertEqual(response.type, int)
	
	def test_get_type_float(self):
		"""
		Test that the type of a float is float.
		"""
		self.client.set("test_type_float", 123.456)
		response = self.client.get("test_type_float")
		self.assertEqual(response.type, float)
	
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
		self.client.set("test_type_str", "test_type_str_value")
		response = self.client.type("test_type_str")
		self.assertEqual(response.type, str)
		self.assertIsNone(response.value)
	
	def test_type_int(self):
		"""
		Test that we can check if a value is an integer.
		"""
		self.client.set("test_type_int", 123)
		response = self.client.type("test_type_int")
		self.assertEqual(response.type, int)
		self.assertIsNone(response.value)
	
	def test_type_float(self):
		"""
		Test that we can check if a value is a float.
		"""
		self.client.set("test_type_float", 123.456)
		response = self.client.type("test_type_float")
		self.assertEqual(response.type, float)
		self.assertIsNone(response.value)
	
	def test_exists(self):
		"""
		Test that we can check if a key exists.
		"""
		self.client.set("test_exists", "test_exists_value")
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
		response = self.client.set("test_exists_delete", "test_exists_delete_value")
		self.assertTrue(response.is_ok())
		response = self.client.delete("test_exists_delete")
		self.assertTrue(response.is_ok())
		response = self.client.exists("test_exists_delete")
		self.assertTrue(response.is_null())
	

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
		response = self.client.set("test_switch_db", "test_switch_db_value")
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
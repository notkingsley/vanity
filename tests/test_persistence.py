import os
import unittest

from client.server_handle import ServerHandle
from tests.utils import get_free_port, make_client


class NoPersistenceTest(unittest.TestCase):
	"""
	Test no persistence across reboots
	"""
	def setUp(self) -> None:
		self.port = get_free_port()
		self.server_handle = ServerHandle(port= self.port, no_db_persist= True)
		self.server_handle.start()

	def tearDown(self) -> None:
		self.server_handle.stop()

	def test_no_db_persist(self):
		"""
		Test that we can set a value on no_db_persist, restart the server, and get a null.
		"""
		with make_client(self.port) as client:
			client.str_set("test_no_db_persist", "test_no_db_persist_value")
			response = client.persist()
			self.assertTrue(response.is_error())

		self.server_handle.restart()

		with make_client(self.port) as client:
			response = client.get("test_no_db_persist")
			self.assertTrue(response.is_null())


class PersistenceTest(unittest.TestCase):
	"""
	Test persistence across reboots
	"""
	def setUp(self) -> None:
		self.tmp_file = os.getcwd() + "/" + "tmp.db"
		self.port = get_free_port()
		self.server_handle = ServerHandle(
			port= self.port,
			no_db_persist= False,
			persist_file= self.tmp_file,
		)
		self.server_handle.start()

	def tearDown(self) -> None:
		self.server_handle.stop()
		os.remove(self.tmp_file)

	def test_persist(self):
		"""
		Test that we can set a value on persist, restart the server, and get the value.
		"""
		with make_client(self.port) as client:
			client.str_set("test_persist", "test_persist_value")
			response = client.persist()
			self.assertTrue(response.is_ok())

		self.server_handle.restart()

		with make_client(self.port) as client:
			response = client.get("test_persist")
			self.assertEqual(response.value, "test_persist_value")
	
	def test_persist_int(self):
		"""
		Test that we can set an integer value on persist,
		restart the server, and get the value.
		"""
		with make_client(self.port) as client:
			client.int_set("test_persist_int", 123)
			response = client.persist()
			self.assertTrue(response.is_ok())

		self.server_handle.restart()

		with make_client(self.port) as client:
			response = client.get("test_persist_int")
			self.assertEqual(response.value, 123)
	
	def test_persist_float(self):
		"""
		Test that we can set a float value on persist,
		restart the server, and get the value.
		"""
		with make_client(self.port) as client:
			client.float_set("test_persist_float", 123.456)
			response = client.persist()
			self.assertTrue(response.is_ok())

		self.server_handle.restart()

		with make_client(self.port) as client:
			response = client.get("test_persist_float")
			self.assertEqual(response.value, 123.456)
	
	def test_persist_list(self):
		"""
		Test that we can set a list value on persist,
		restart the server, and get the value.
		"""
		with make_client(self.port) as client:
			client.list_push_right("test_persist_list", ["red", "green", "blue"])
			response = client.persist()
			self.assertTrue(response.is_ok())

		self.server_handle.restart()

		with make_client(self.port) as client:
			response = client.list_range("test_persist_list", 0, -1)
			self.assertEqual(response.value, ["red", "green", "blue"])
	
	def test_persist_set(self):
		"""
		Test that we can set a set value on persist,
		restart the server, and get the value.
		"""
		with make_client(self.port) as client:
			client.set_add("test_persist_set", {"red", "green", "blue"})
			response = client.persist()
			self.assertTrue(response.is_ok())

		self.server_handle.restart()

		with make_client(self.port) as client:
			response = client.set_all("test_persist_set")
			self.assertEqual(response.value, {"red", "green", "blue"})
	
	def test_persist_hash(self):
		"""
		Test that we can set a hash value on persist,
		restart the server, and get the value.
		"""
		with make_client(self.port) as client:
			response = client.hash_set("test_persist_hash", {"red": "1", "green": "2", "blue": "3"})
			self.assertTrue(response.is_ok())
			response = client.persist()
			self.assertTrue(response.is_ok())

		self.server_handle.restart()

		with make_client(self.port) as client:
			response = client.hash_all("test_persist_hash")
			self.assertEqual(response.value, {"red": "1", "green": "2", "blue": "3"})
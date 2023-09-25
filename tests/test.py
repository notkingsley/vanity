import os
import unittest

from client.client import Client
from client.server_handle import ServerHandle


TEST_PORT = 19955


def make_client() -> Client:
	"""
	Make a client.
	"""
	return Client("localhost", TEST_PORT)


def make_server_handle(port= TEST_PORT, **kwargs) -> ServerHandle:
	"""
	Make a server handle.
	"""
	return ServerHandle(port= port, **kwargs)


class KeyValueStoreTest(unittest.TestCase):
	server_handle: ServerHandle
	client: Client

	@classmethod
	def setUpClass(cls) -> None:
		cls.server_handle = make_server_handle()
		cls.server_handle.start()
		cls.client = make_client()
	
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
	

class NoPersistenceTest(unittest.TestCase):
	"""
	Test no persistence across reboots
	"""
	def setUp(self) -> None:
		self.server_handle = make_server_handle(no_persist= True)
		self.server_handle.start()

	def tearDown(self) -> None:
		self.server_handle.stop()

	def test_no_persist(self):
		"""
		Test that we can set a value on no_persist, restart the server, and get a null.
		"""
		with make_client() as client:
			client.set("test_no_persist", "test_no_persist_value")
			response = client.persist()
			self.assertTrue(response.is_error())

		self.server_handle.restart()

		with make_client() as client:
			response = client.get("test_no_persist")
			self.assertTrue(response.is_null())


class PersistenceTest(unittest.TestCase):
	"""
	Test persistence across reboots
	"""
	TMP_FILE = os.getcwd() + "/" + "tmp.db"

	def setUp(self) -> None:
		self.server_handle = make_server_handle(
			no_persist= False,
			persist_file= self.TMP_FILE,
		)
		self.server_handle.start()

	def tearDown(self) -> None:
		self.server_handle.stop()
		os.remove(self.TMP_FILE)

	def test_persist(self):
		"""
		Test that we can set a value on persist, restart the server, and get the value.
		"""
		with make_client() as client:
			client.set("test_persist", "test_persist_value")
			response = client.persist()
			self.assertTrue(response.is_ok())

		self.server_handle.restart()

		with make_client() as client:
			response = client.get("test_persist")
			self.assertEqual(response.value, "test_persist_value")
	
	def test_persist_int(self):
		"""
		Test that we can set an integer value on persist, restart the server, and get the value.
		"""
		with make_client() as client:
			client.set("test_persist_int", 123)
			response = client.persist()
			self.assertTrue(response.is_ok())

		self.server_handle.restart()

		with make_client() as client:
			response = client.get("test_persist_int")
			self.assertEqual(response.value, 123)
	
	def test_persist_float(self):
		"""
		Test that we can set a float value on persist, restart the server, and get the value.
		"""
		with make_client() as client:
			client.set("test_persist_float", 123.456)
			response = client.persist()
			self.assertTrue(response.is_ok())

		self.server_handle.restart()

		with make_client() as client:
			response = client.get("test_persist_float")
			self.assertEqual(response.value, 123.456)
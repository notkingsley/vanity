import unittest

from client.client import Client


TEST_HOST = "localhost"
TEST_PORT = 9955


class KeyValueStoreTest(unittest.TestCase):
	def setUp(self):
		self.client = Client(TEST_HOST, TEST_PORT)
	
	def tearDown(self):
		self.client.close()

	def test_set_get(self):
		"""
		Test that we can set a value and then get it.
		"""
		self.client.set("test_set_get_key", "test_set_get_value")
		response = self.client.get("test_set_get_key")
		self.assertEqual(response.body, "test_set_get_value")

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
		self.assertEqual(response.body, "test_set_overwrite_value2")
	
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
		self.assertEqual(response.body, "test_set_delete_set_get_value2")
	
	def test_ping(self):
		"""
		Test that we can ping the server.
		"""
		response = self.client.request("PING")
		self.assertTrue(response.is_pong())
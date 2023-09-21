import unittest

from client.client import Client


TEST_HOST = "localhost"
TEST_PORT = 9954


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

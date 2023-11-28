import time

from tests.test_db import BaseDatabaseTest


class ExpiryTest(BaseDatabaseTest):
	"""
	Test the expiry commands
	"""
	EXPIRE_TIME = 0.1

	def test_set_expiry(self):
		"""
		Test the set_expiry command.
		"""
		response = self.client.str_set("key", "value")
		self.assertTrue(response.is_ok())
		response = self.client.set_expiry("key", self.EXPIRE_TIME)
		self.assertTrue(response.is_ok())
		time.sleep(self.EXPIRE_TIME)
		response = self.client.get("key")
		self.assertTrue(response.is_null())
	
	def test_get_expiry(self):
		"""
		Test the get_expiry command.
		"""
		response = self.client.str_set("key", "value")
		self.assertTrue(response.is_ok())
		response = self.client.set_expiry("key", self.EXPIRE_TIME)
		self.assertTrue(response.is_ok())
		response = self.client.get_expiry("key")
		self.assertTrue(response.is_ok())
		self.assertAlmostEqual(response.value, self.EXPIRE_TIME, 2)
	
	def test_clear_expiry(self):
		"""
		Test the clear_expiry command.
		"""
		response = self.client.str_set("key", "value")
		self.assertTrue(response.is_ok())
		response = self.client.set_expiry("key", self.EXPIRE_TIME)
		self.assertTrue(response.is_ok())
		response = self.client.clear_expiry("key")
		self.assertTrue(response.is_ok())
		time.sleep(self.EXPIRE_TIME)
		response = self.client.get("key")
		self.assertEqual(response.value, "value")
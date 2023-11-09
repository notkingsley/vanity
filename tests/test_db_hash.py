from tests.test_db import BaseDatabaseTest


class NonExistentKeyTest(BaseDatabaseTest):
	"""
	Test that a non-existent key behaves like an empty set
	"""

	def test_hash_all(self):
		"""
		Test that the hash_all command returns an empty dict
		"""
		response = self.client.hash_all("test_hash_all")
		self.assertEqual(response.value, {})

	def test_hash_get(self):
		"""
		Test that the hash_get command returns a bad key error
		"""
		response = self.client.hash_get("test_hash_get", "field")
		self.assertTrue(response.is_error())
		self.assertEqual(response.body, "bad key")

	def test_hash_contains(self):
		"""
		Test that the hash_contains command returns False
		"""
		response = self.client.hash_contains("test_hash_contains", "field")
		self.assertEqual(response.value, False)

	def test_hash_len(self):
		"""
		Test that the hash_len command returns 0
		"""
		response = self.client.hash_len("test_hash_len")
		self.assertEqual(response.value, 0)

	def test_hash_key_len(self):
		"""
		Test that the hash_key_len command returns a bad key error
		"""
		response = self.client.hash_key_len("test_hash_key_len", "field")
		self.assertTrue(response.is_error())
		self.assertEqual(response.body, "bad key")

	def test_hash_remove(self):
		"""
		Test that the hash_remove command returns 0
		"""
		response = self.client.hash_remove("test_hash_remove", ["field1", "field2"])
		self.assertEqual(response.value, 0)

	def test_hash_keys(self):
		"""
		Test that the hash_keys command returns an empty list
		"""
		response = self.client.hash_keys("test_hash_keys")
		self.assertEqual(response.value, [])

	def test_hash_values(self):
		"""
		Test that the hash_values command returns an empty list
		"""
		response = self.client.hash_values("test_hash_values")
		self.assertEqual(response.value, [])

	def test_hash_many_get(self):
		"""
		Test that the hash_many_get command returns a list of Nones
		"""
		response = self.client.hash_many_get("test_hash_many_get", ["field1", "field2"])
		self.assertEqual(response.value, [None, None])


class BadTypeTest(BaseDatabaseTest):
	"""
	Test hash commands on a non-hash value
	"""

	def setUp(self) -> None:
		super().setUp()
		response = self.client.set("test_bad_type", "value")
		self.assertTrue(response.is_ok())

	def test_hash_all(self):
		"""
		Test that the hash_all command returns a bad type error
		"""
		response = self.client.hash_all("test_bad_type")
		self.assertTrue(response.is_bad_type())
		self.assertEqual(response.body, "not hash")

	def test_hash_get(self):
		"""
		Test that the hash_get command returns a bad type error
		"""
		response = self.client.hash_get("test_bad_type", "field")
		self.assertTrue(response.is_bad_type())
		self.assertEqual(response.body, "not hash")

	def test_hash_contains(self):
		"""
		Test that the hash_contains command returns a bad type error
		"""
		response = self.client.hash_contains("test_bad_type", "field")
		self.assertTrue(response.is_bad_type())
		self.assertEqual(response.body, "not hash")

	def test_hash_len(self):
		"""
		Test that the hash_len command returns a bad type error
		"""
		response = self.client.hash_len("test_bad_type")
		self.assertTrue(response.is_bad_type())
		self.assertEqual(response.body, "not hash")

	def test_hash_key_len(self):
		"""
		Test that the hash_key_len command returns a bad type error
		"""
		response = self.client.hash_key_len("test_bad_type", "field")
		self.assertTrue(response.is_bad_type())
		self.assertEqual(response.body, "not hash")

	def test_hash_remove(self):
		"""
		Test that the hash_remove command returns a bad type error
		"""
		response = self.client.hash_remove("test_bad_type", ["field1", "field2"])
		self.assertTrue(response.is_bad_type())
		self.assertEqual(response.body, "not hash")

	def test_hash_keys(self):
		"""
		Test that the hash_keys command returns a bad type error
		"""
		response = self.client.hash_keys("test_bad_type")
		self.assertTrue(response.is_bad_type())
		self.assertEqual(response.body, "not hash")

	def test_hash_values(self):
		"""
		Test that the hash_values command returns a bad type error
		"""
		response = self.client.hash_values("test_bad_type")
		self.assertTrue(response.is_bad_type())
		self.assertEqual(response.body, "not hash")

	def test_hash_many_get(self):
		"""
		Test that the hash_many_get command returns a bad type error
		"""
		response = self.client.hash_many_get("test_bad_type", ["field1", "field2"])
		self.assertTrue(response.is_bad_type())
		self.assertEqual(response.body, "not hash")


class HashTest(BaseDatabaseTest):
	"""
	Test the hash commands
	"""

	def setUp(self) -> None:
		super().setUp()
		response = self.client.hash_set(
			"test_hash_set",
			{
				"field1": "value1",
				"field2": "value2",
				"field3": "value3",
				"field4": "value4",
				"field5": "value5",
				"field6": "value6",
			},
		)
		self.assertTrue(response.is_ok())
	
	def test_hash_set(self):
		"""
		Test that the hash_set command sets a value
		"""
		response = self.client.hash_set("test_hash_set", {"field7": "value7"})
		self.assertTrue(response.is_ok())
		response = self.client.hash_all("test_hash_set")
		self.assertEqual(response.value, {"field7": "value7"})
	
	def test_hash_all(self):
		"""
		Test that the hash_all command returns all fields
		"""
		response = self.client.hash_all("test_hash_set")
		self.assertEqual(
			response.value,
			{
				"field1": "value1",
				"field2": "value2",
				"field3": "value3",
				"field4": "value4",
				"field5": "value5",
				"field6": "value6",
			},
		)
	
	def test_hash_get(self):
		"""
		Test that the hash_get command returns a single field
		"""
		response = self.client.hash_get("test_hash_set", "field1")
		self.assertEqual(response.value, "value1")
	
	def test_hash_get_missing(self):
		"""
		Test that the hash_get command returns a bad key error for a missing field
		"""
		response = self.client.hash_get("test_hash_set", "field7")
		self.assertTrue(response.is_error())
		self.assertEqual(response.body, "bad key")
	
	def test_hash_contains(self):
		"""
		Test that the hash_contains command returns True if the field exists
		"""
		response = self.client.hash_contains("test_hash_set", "field1")
		self.assertEqual(response.value, True)
	
	def test_hash_contains_false(self):
		"""
		Test that the hash_contains command returns False if the field does not exist
		"""
		response = self.client.hash_contains("test_hash_set", "field7")
		self.assertEqual(response.value, False)
	
	def test_hash_len(self):
		"""
		Test that the hash_len command returns the number of fields
		"""
		response = self.client.hash_len("test_hash_set")
		self.assertEqual(response.value, 6)
	
	def test_hash_key_len(self):
		"""
		Test that the hash_key_len command returns the length of a field
		"""
		response = self.client.hash_key_len("test_hash_set", "field1")
		self.assertEqual(response.value, 6)
	
	def test_hash_key_len_missing(self):
		"""
		Test that the hash_key_len command returns 0 for a missing field
		"""
		response = self.client.hash_key_len("test_hash_set", "field7")
		self.assertTrue(response.is_error())
		self.assertEqual(response.body, "bad key")
	
	def test_hash_remove(self):
		"""
		Test that the hash_remove command removes a field
		"""
		response = self.client.hash_remove("test_hash_set", ["field1", "field2"])
		self.assertEqual(response.value, 2)
		response = self.client.hash_all("test_hash_set")
		self.assertEqual(
			response.value,
			{
				"field3": "value3",
				"field4": "value4",
				"field5": "value5",
				"field6": "value6",
			},
		)
	
	def test_hash_remove_missing(self):
		"""
		Test that the hash_remove command removes only existing fields
		"""
		response = self.client.hash_remove("test_hash_set", ["field4", "field7"])
		self.assertEqual(response.value, 1)
	
	def test_hash_keys(self):
		"""
		Test that the hash_keys command returns all fields
		"""
		response = self.client.hash_keys("test_hash_set")
		self.assertEqual(
			set(response.value),
			{"field1", "field2", "field3", "field4", "field5", "field6"},
		)
	
	def test_hash_values(self):
		"""
		Test that the hash_values command returns all values
		"""
		response = self.client.hash_values("test_hash_set")
		self.assertEqual(
			set(response.value),
			{"value1", "value2", "value3", "value4", "value5", "value6"},
		)
	
	def test_hash_many_get(self):
		"""
		Test that the hash_many_get command returns multiple fields
		"""
		response = self.client.hash_many_get(
			"test_hash_set", ["field1", "field2", "field3"]
		)
		self.assertEqual(response.value, ["value1", "value2", "value3"])
	
	def test_hash_many_get_missing(self):
		"""
		Test that the hash_many_get command returns None for missing fields
		"""
		response = self.client.hash_many_get(
			"test_hash_set", ["field1", "field2", "field7"]
		)
		self.assertEqual(response.value, ["value1", "value2", None])
	
	def test_hash_many_get_empty(self):
		"""
		Test that the hash_many_get command returns an empty list for an empty list of fields
		"""
		response = self.client.hash_many_get("test_hash_set", [])
		self.assertEqual(response.value, [])
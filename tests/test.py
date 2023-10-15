import os
import socket
import unittest

from client.client import Client, AuthLevel
from client.server_handle import ServerHandle


def get_free_port():
	"""
	Get a (likely) free port number.
	"""
	s = socket.socket()
	s.bind(('', 0))
	port = s.getsockname()[1]
	s.close()
	return port


def make_client(port, **kwargs) -> Client:
	"""
	Make a client.
	"""
	return Client("localhost", port, **kwargs)


def make_server_handle(port, **kwargs) -> ServerHandle:
	"""
	Make a server handle.
	"""
	return ServerHandle(port= port, **kwargs)


class KeyValueStoreTest(unittest.TestCase):
	server_handle: ServerHandle
	client: Client

	@classmethod
	def setUpClass(cls) -> None:
		port = get_free_port()
		cls.server_handle = make_server_handle(port)
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
	

class NoPersistenceTest(unittest.TestCase):
	"""
	Test no persistence across reboots
	"""
	def setUp(self) -> None:
		self.port = get_free_port()
		self.server_handle = make_server_handle(port= self.port, no_persist= True)
		self.server_handle.start()

	def tearDown(self) -> None:
		self.server_handle.stop()

	def test_no_persist(self):
		"""
		Test that we can set a value on no_persist, restart the server, and get a null.
		"""
		with make_client(self.port) as client:
			client.set("test_no_persist", "test_no_persist_value")
			response = client.persist()
			self.assertTrue(response.is_error())

		self.server_handle.restart()

		with make_client(self.port) as client:
			response = client.get("test_no_persist")
			self.assertTrue(response.is_null())


class PersistenceTest(unittest.TestCase):
	"""
	Test persistence across reboots
	"""
	def setUp(self) -> None:
		self.tmp_file = os.getcwd() + "/" + "tmp.db"
		self.port = get_free_port()
		self.server_handle = make_server_handle(
			port= self.port,
			no_persist= False,
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
			client.set("test_persist", "test_persist_value")
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
			client.set("test_persist_int", 123)
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
			client.set("test_persist_float", 123.456)
			response = client.persist()
			self.assertTrue(response.is_ok())

		self.server_handle.restart()

		with make_client(self.port) as client:
			response = client.get("test_persist_float")
			self.assertEqual(response.value, 123.456)


class UnknownUserTest(unittest.TestCase):
	"""
	Test no authentication
	"""
	@classmethod
	def setUpClass(cls) -> None:
		cls.port = get_free_port()
		cls.server_handle = make_server_handle(cls.port)
		cls.server_handle.start()

	@classmethod
	def tearDownClass(cls) -> None:
		cls.server_handle.stop()

	def setUp(self) -> None:
		self.client = make_client(self.port, no_login= True)

	def tearDown(self) -> None:
		self.client.close()
		
	def test_get_no_login(self):
		"""
		Test that we can't get data from the server if we don't login.
		"""
		response = self.client.get("test_get_no_login")
		self.assertTrue(response.is_denied())
	
	def test_set_no_login(self):
		"""
		Test that we can't set data on the server if we don't login.
		"""
		response = self.client.set("test_set_no_login", "test_set_no_login_value")
		self.assertTrue(response.is_denied())
	
	def test_delete_no_login(self):
		"""
		Test that we can't delete data from the server if we don't login.
		"""
		response = self.client.delete("test_delete_no_login")
		self.assertTrue(response.is_denied())
	
	def test_persist_no_login(self):
		"""
		Test that we can't persist data on the server if we don't login.
		"""
		response = self.client.persist()
		self.assertTrue(response.is_denied())
	
	def test_reset_no_login(self):
		"""
		Test that we can't reset the server if we don't login.
		"""
		response = self.client.reset()
		self.assertTrue(response.is_denied())
	
	def test_terminate_no_login(self):
		"""
		Test that we can't terminate the server if we don't login.
		"""
		self.client.terminate()
		self.assertTrue(self.server_handle.is_running())
		response = self.client.read_response()
		self.assertTrue(response.is_denied())
	
	def test_ping_no_login(self):
		"""
		Test that we can ping the server if we don't login.
		"""
		response = self.client.ping()
		self.assertTrue(response.is_pong())
	
	def test_switch_db_no_login(self):
		"""
		Test that we can't switch databases if we don't login.
		"""
		response = self.client.switch_db(1)
		self.assertTrue(response.is_denied())
	
	def test_add_user_no_login(self):
		"""
		Test that we can't add a user if we don't login.
		"""
		response = self.client.add_user("test_add_user_no_login", "test_add_user_no_login_password")
		self.assertTrue(response.is_denied())
	
	def test_edit_user_no_login(self):
		"""
		Test that we can't edit a user if we don't login.
		"""
		response = self.client.edit_user("test_edit_user_no_login", AuthLevel.ADMIN)
		self.assertTrue(response.is_denied())
	
	def test_del_user_no_login(self):
		"""
		Test that we can't delete a user if we don't login.
		"""
		response = self.client.del_user("test_del_user_no_login")
		self.assertTrue(response.is_denied())

	def test_change_password_no_login(self):
		"""
		Test that we can't change a user's password if we don't login.
		"""
		response = self.client.change_password("test_change_password_no_login")
		self.assertTrue(response.is_denied())


class UserAuthTest(unittest.TestCase):
	"""
	Test the authentication for a user
	"""
	@classmethod
	def setUpClass(cls) -> None:
		cls.port = get_free_port()
		cls.server_handle = make_server_handle(cls.port)
		cls.server_handle.start()
		cls.admin_client = make_client(cls.port)
		cls.admin_client.add_user("test_user_auth", "test_user_auth_password")
		cls.admin_client.edit_user("test_user_auth", AuthLevel.USER)
		cls.client = make_client(cls.port, no_login= True)
		cls.client.auth("test_user_auth", "test_user_auth_password")

	@classmethod
	def tearDownClass(cls) -> None:
		cls.client.close()
		cls.admin_client.close()
		cls.server_handle.stop()
	
	def test_get(self):
		"""
		Test that we can get data from the server if we login.
		"""
		response = self.client.get("test_get")
		self.assertTrue(response.is_null())
	
	def test_set(self):
		"""
		Test that we can set data on the server if we login.
		"""
		response = self.client.set("test_set", "test_set_value")
		self.assertTrue(response.is_ok())
		response = self.client.get("test_set")
		self.assertTrue(response.is_ok())
		self.assertEqual(response.value, "test_set_value")
	
	def test_delete(self):
		"""
		Test that we can delete data from the server if we login.
		"""
		response = self.client.delete("test_delete")
		self.assertTrue(response.is_error())
	
	def test_reset(self):
		"""
		Test that we can't reset the server if we login.
		"""
		response = self.client.reset()
		self.assertTrue(response.is_denied())
	
	def test_terminate(self):
		"""
		Test that we can't terminate the server if we login.
		"""
		self.client.terminate()
		self.assertTrue(self.server_handle.is_running())
		response = self.client.read_response()
		self.assertTrue(response.is_denied())
	
	def test_ping(self):
		"""
		Test that we can ping the server if we login.
		"""
		response = self.client.ping()
		self.assertTrue(response.is_pong())
	
	def test_switch_db(self):
		"""
		Test that we can switch databases if we login.
		"""
		response = self.client.switch_db(1)
		self.assertTrue(response.is_ok())
	
	def test_add_user(self):
		"""
		Test that we can't add a user if we login.
		"""
		response = self.client.add_user("test_add_user", "test_add_user_password")
		self.assertTrue(response.is_denied())
	
	def test_edit_user(self):
		"""
		Test that we can't edit a user if we login.
		"""
		response = self.client.edit_user("test_edit_user", AuthLevel.ADMIN)
		self.assertTrue(response.is_denied())
	
	def test_del_user(self):
		"""
		Test that we can't delete a user if we login.
		"""
		response = self.client.del_user("test_del_user")
		self.assertTrue(response.is_denied())
	
	def test_change_password(self):
		"""
		Test that we can change the password if we login.
		"""
		response = self.client.change_password("test_change_password")
		self.assertTrue(response.is_ok())
		with make_client(self.port, no_login= True) as client:
			response = client.auth("test_user_auth", "test_change_password")
			self.assertTrue(response.is_ok())


class DefaultAuthTest(unittest.TestCase):
	"""
	Test the default authentication
	"""
	@classmethod
	def setUpClass(cls) -> None:
		cls.port = get_free_port()
		cls.server_handle = make_server_handle(cls.port)
		cls.server_handle.start()

	@classmethod
	def tearDownClass(cls) -> None:
		cls.server_handle.stop()

	def test_auth(self):
		"""
		Test that we can authenticate with the server with the default login.
		"""
		with make_client(self.port, no_login= True) as client:
			response = client.auth(
				client.DEFAULT_ADMIN_USERNAME,
				client.DEFAULT_ADMIN_PASSWORD,
			)
			self.assertTrue(response.is_ok())
	
	def test_auth_fail(self):
		"""
		Test that we can't authenticate with the server with the wrong password.
		"""
		with make_client(self.port, no_login= True) as client:
			response = client.auth("invalid_username", "invalid_password")
			self.assertTrue(response.is_error())
	
	def test_default_admin(self):
		"""
		Test that the default login is an admin.
		"""
		with make_client(self.port, no_login= True) as client:
			response = client.auth(
				client.DEFAULT_ADMIN_USERNAME,
				client.DEFAULT_ADMIN_PASSWORD,
			)
			self.assertTrue(response.is_ok())
			response = client.edit_user("invalid_username", AuthLevel.ADMIN)
			self.assertFalse(response.is_denied())
	

class SwitchDBTest(unittest.TestCase):
	"""
	Test database index switching (incomplete)
	"""
	@classmethod
	def setUpClass(cls) -> None:
		cls.port = get_free_port()
		cls.server_handle = make_server_handle(cls.port)
		cls.server_handle.start()

	@classmethod
	def tearDownClass(cls) -> None:
		cls.server_handle.stop()

	def setUp(self) -> None:
		self.client = make_client(self.port)
	
	def tearDown(self) -> None:
		self.client.close()
	
	@unittest.skip("Not implemented")
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
	

class AccountsTest(unittest.TestCase):
	"""
	Test that we can create accounts and edit their permissions.
	"""
	@classmethod
	def setUpClass(cls) -> None:
		cls.port = get_free_port()
		cls.server_handle = make_server_handle(cls.port)
		cls.server_handle.start()
		cls.client = make_client(cls.port)

	@classmethod
	def tearDownClass(cls) -> None:
		cls.client.close()
		cls.server_handle.stop()
	
	def test_add_user(self):
		"""
		Test that we can add a user.
		"""
		response = self.client.add_user("test_add_user", "test_add_user_password")
		self.assertTrue(response.is_ok())
		with make_client(self.port, no_login= True) as client:
			response = client.auth("test_add_user", "test_add_user_password")
			self.assertTrue(response.is_ok())
	
	def test_add_user_fail(self):
		"""
		Test that we can't add a user with an existing username.
		"""
		response = self.client.add_user("test_add_user_fail", "test_add_user_fail_password")
		self.assertTrue(response.is_ok())
		response = self.client.add_user("test_add_user_fail", "test_add_user_fail_password")
		self.assertTrue(response.is_error())
	
	def test_default_add_user_is_not_admin(self):
		"""
		Test that a user added with the default login is not an admin.
		"""
		response = self.client.add_user("test_default_add_user_is_not_admin", "test_default_add_user_is_not_admin_password")
		self.assertTrue(response.is_ok())
		with make_client(self.port, no_login= True) as client:
			response = client.auth("test_default_add_user_is_not_admin", "test_default_add_user_is_not_admin_password")
			self.assertTrue(response.is_ok())
			response = client.edit_user("invalid_username", AuthLevel.ADMIN)
			self.assertTrue(response.is_denied())
	
	def test_add_user_fail_if_not_admin(self):
		"""
		Test that we can't add a user if we're not an admin.
		"""
		response = self.client.add_user("test_add_user_fail_if_not_admin", "test_add_user_fail_if_not_admin_password")
		self.assertTrue(response.is_ok())
		response = self.client.edit_user("test_add_user_fail_if_not_admin", AuthLevel.USER)
		self.assertTrue(response.is_ok())
		with make_client(self.port, no_login= True) as client:
			response = client.auth("test_add_user_fail_if_not_admin", "test_add_user_fail_if_not_admin_password")
			self.assertTrue(response.is_ok())
			response = client.add_user("test_add_user_fail_if_not_admin2", "test_add_user_fail_if_not_admin2_password")
			self.assertTrue(response.is_denied())
	
	def test_edit_user(self):
		"""
		Test that we can edit a user and make them admin.
		"""
		response = self.client.add_user("test_edit_user", "test_edit_user_password")
		self.assertTrue(response.is_ok())
		response = self.client.edit_user("test_edit_user", AuthLevel.ADMIN)
		self.assertTrue(response.is_ok())
		with make_client(self.port, no_login= True) as client:
			response = client.auth("test_edit_user", "test_edit_user_password")
			self.assertTrue(response.is_ok())
			response = client.edit_user("invalid_username", AuthLevel.ADMIN)
			self.assertFalse(response.is_denied())
	
	def test_edit_user_fail(self):
		"""
		Test that we can't edit a user that doesn't exist.
		"""
		response = self.client.edit_user("test_edit_user_fail", AuthLevel.ADMIN)
		self.assertTrue(response.is_error())
	
	def test_edit_user_fail_if_not_admin(self):
		"""
		Test that we can't edit a user if we're not an admin.
		"""
		response = self.client.add_user("test_edit_user_fail_if_not_admin", "test_edit_user_fail_if_not_admin_password")
		self.assertTrue(response.is_ok())
		response = self.client.edit_user("test_edit_user_fail_if_not_admin", AuthLevel.USER)
		self.assertTrue(response.is_ok())
		with make_client(self.port, no_login= True) as client:
			response = client.auth("test_edit_user_fail_if_not_admin", "test_edit_user_fail_if_not_admin_password")
			self.assertTrue(response.is_ok())
			response = client.edit_user("test_edit_user_fail_if_not_admin", AuthLevel.ADMIN)
			self.assertTrue(response.is_denied())
	
	def test_edit_user_auth_levels(self):
		"""
		Test that we can only edit users to be USER, or ADMIN.
		"""
		response = self.client.add_user("test_edit_user_auth_levels", "test_edit_user_auth_levels_password")
		self.assertTrue(response.is_ok())
		response = self.client.edit_user("test_edit_user_auth_levels", AuthLevel.USER)
		self.assertTrue(response.is_ok())
		response = self.client.edit_user("test_edit_user_auth_levels", AuthLevel.ADMIN)
		self.assertTrue(response.is_ok())
		response = self.client.edit_user("test_edit_user_auth_levels", AuthLevel.PEER)
		self.assertTrue(response.is_error())
		response = self.client.edit_user("test_edit_user_auth_levels", AuthLevel.UNKNOWN)
		self.assertTrue(response.is_error())
	
	def test_del_user(self):
		"""
		Test that we can delete a user.
		"""
		response = self.client.add_user("test_del_user", "test_del_user_password")
		self.assertTrue(response.is_ok())
		response = self.client.del_user("test_del_user")
		self.assertTrue(response.is_ok())
		with make_client(self.port, no_login= True) as client:
			response = client.auth("test_del_user", "test_del_user_password")
			self.assertTrue(response.is_error())
	
	def test_del_user_fail(self):
		"""
		Test that we can't delete a user that doesn't exist.
		"""
		response = self.client.del_user("test_del_user_fail")
		self.assertTrue(response.is_error())
	
	def test_del_user_fail_if_not_admin(self):
		"""
		Test that we can't delete a user if we're not an admin.
		"""
		response = self.client.add_user("test_del_user_fail_if_not_admin", "test_del_user_fail_if_not_admin_password")
		self.assertTrue(response.is_ok())
		response = self.client.edit_user("test_del_user_fail_if_not_admin", AuthLevel.USER)
		self.assertTrue(response.is_ok())
		response = self.client.add_user("test_del_user_fail_if_not_admin2", "test_del_user_fail_if_not_admin2_password")
		self.assertTrue(response.is_ok())
		with make_client(self.port, no_login= True) as client:
			response = client.auth("test_del_user_fail_if_not_admin", "test_del_user_fail_if_not_admin_password")
			self.assertTrue(response.is_ok())
			response = client.del_user("test_del_user_fail_if_not_admin2")
			self.assertTrue(response.is_denied())
	
	def test_del_user_fail_if_self(self):
		"""
		Test that we can't delete ourselves.
		"""
		response = self.client.del_user("test_del_user_fail_if_self")
		self.assertTrue(response.is_error())

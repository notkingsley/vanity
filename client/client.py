from enum import Enum

from client.response import Response
from client.socket_client import SocketClient


class AuthLevel(Enum):
	"""
	Auth levels for users.
	"""
	UNKNOWN = "UNKNOWN"
	USER = "USER"
	PEER = "PEER"
	ADMIN = "ADMIN"


def format(msg: str | int | float | list | AuthLevel) -> str:
	"""
	Format a message body to be sent to the server.
	Escape the quotes in the message if it is a string
	:param msg: The message to format.
	:return: The formatted message.
	"""
	if isinstance(msg, (int, float)):
		return str(msg)
	elif isinstance(msg, AuthLevel):
		return msg.value
	elif isinstance(msg, list):
		return f"({len(msg)})[" + "".join(map(format, msg)) + "]"
	elif isinstance(msg, str):
		return f"({len(msg)})" + msg
	else:
		raise TypeError(f"Cannot format {msg}.")


class Client:
	"""
	A Client provides a programmatic interface for interacting with a Vanity Server.
	"""
	DEFAULT_ADMIN_USERNAME = "vanity"
	DEFAULT_ADMIN_PASSWORD = "vanity"

	def __init__(
		self,
		host: str,
		port: int,
		*,
		no_login: bool = False,
		username: str | None = DEFAULT_ADMIN_USERNAME,
		password: str | None = DEFAULT_ADMIN_PASSWORD,
		db_index: int | None = None,
	):
		"""
		Connect to a Vanity Server.
		:param host: The host to connect to.
		:param port: The port to connect to.
		:param no_login: Whether to skip the login process.
		:param username: The username to login with.
		:param password: The password to login with.
		:param db_index: The database index to switch to.
		"""
		self.sock = SocketClient(host, port)

		if not no_login:
			self.auth(username, password)

		if db_index is not None:
			self.switch_db(db_index)
	
	def __enter__(self):
		return self
	
	def __exit__(self, exc_type, exc_val, exc_tb):
		self.close()
	
	def close(self):
		"""
		Close the connection to the server.
		"""
		self.sock.close()
	
	def send_command(self, command: str, *args):
		"""
		Send a command to the server.
		:param command: The command to send.
		:param args: The arguments to send.
		"""
		self.sock.send(f"{command} {' '.join(map(format, args))}")
	
	def read_response(self) -> Response:
		"""
		Read a response from the server.
		:return: The response from the server.
		"""
		return Response(self.sock.read_msg())
	
	def request(self, command: str, *args) -> Response:
		"""
		Send a request to the server.
		:param command: The command to send.
		:param args: The arguments to send.
		:return: The response from the server.
		"""
		self.send_command(command, *args)
		return self.read_response()
	
	def get(self, key: str):
		"""
		Get the value of a key.
		:param key: The key to get.
		:return: The value of the key.
		"""
		return self.request("GET", key)
	
	def set(self, key: str, value: str | int | float):
		"""
		Set the value of a key.
		:param key: The key to set.
		:param value: The value to set.
		"""
		if isinstance(value, str):
			command = "SET :STR"
		elif isinstance(value, int):
			command = "SET :INT"
		elif isinstance(value, float):
			command = "SET :FLOAT"
		return self.request(command, key, value)
	
	def delete(self, key: str):
		"""
		Delete a key.
		:param key: The key to delete.
		"""
		return self.request("DEL", key)
	
	def exit(self):
		"""
		Request to exit the session
		"""
		self.send_command("EXIT")
	
	def terminate(self):
		"""
		Terminate the server.
		"""
		self.send_command("TERMINATE")
	
	def ping(self, msg = str()):
		"""
		Ping the server.
		"""
		return self.request("PING" + msg)
	
	def persist(self):
		"""
		Persist the database.
		"""
		return self.request("PERSIST")
	
	def reset(self):
		"""
		Reset the database.
		"""
		return self.request("RESET")
	
	def auth(self, username: str, password: str):
		"""
		Authenticate with the server.
		:param username: The username to authenticate with.
		:param password: The password to authenticate with.
		"""
		return self.request("AUTH", username, password)
	
	def switch_db(self, db: int):
		"""
		Switch to a different database index
		:param db: The database to switch to.
		"""
		return self.request("SWITCH_DB", db)
	
	def add_user(self, username: str, password: str):
		"""
		Add a user to the server.
		:param username: The username to add.
		:param password: The password to add.
		"""
		return self.request("ADD_USER", username, password)
	
	def edit_user(self, username: str, auth_level: AuthLevel):
		"""
		Edit a user's auth level on the server.
		:param username: The username to edit.
		:param auth_level: The new auth level.
		"""
		return self.request("EDIT_USER", username, auth_level)
	
	def del_user(self, username: str):
		"""
		Delete a user from the server.
		:param username: The username to delete.
		"""
		return self.request("DEL_USER", username)
	
	def change_password(self, password: str):
		"""
		Change the current client's password.
		:param password: The new password.
		"""
		return self.request("CHANGE_PASSWORD", password)
	
	def type(self, key: str):
		"""
		Get the type of a key.
		:param key: The key to get the type of.
		:return: The type of the key.
		"""
		return self.request("TYPE", key)
	
	def exists(self, key: str):
		"""
		Check if a key exists.
		:param key: The key to check.
		:return: Whether the key exists.
		"""
		return self.request("EXISTS", key)
	
	def incr_int(self, key: str, by: int = 1):
		"""
		Increment an integer value.
		:param key: The key to increment.
		:return: The incremented value.
		"""
		return self.request("INCR_INT", key, by)
	
	def decr_int(self, key: str, by: int = 1):
		"""
		Decrement an integer value.
		:param key: The key to decrement.
		:return: The decremented value.
		"""
		return self.incr_int(key, -by)
	
	def incr_float(self, key: str, by: float = 1.0):
		"""
		Increment a float value.
		:param key: The key to increment.
		:return: The incremented value.
		"""
		return self.request("INCR_FLOAT", key, by)
	
	def decr_float(self, key: str, by: float = 1.0):
		"""
		Decrement a float value.
		:param key: The key to decrement.
		:return: The decremented value.
		"""
		return self.incr_float(key, -by)
	
	def str_len(self, key: str):
		"""
		Get the length of a string.
		:param key: The key to get the length of.
		:return: The length of the string.
		"""
		return self.request("STR_LEN", key)
	
	def many_get(self, *keys):
		"""
		Get the values of many keys.
		:param keys: The keys to get.
		:return: The values of the keys.
		"""
		return self.request("MANY_GET", list(keys))
	
	def list_len(self, key: str):
		"""
		Get the length of a list.
		:param key: The key to get the length of.
		:return: The length of the list.
		"""
		return self.request("LIST_LEN", key)
	
	def list_get(self, key: str, index: int):
		"""
		Get an item from a list.
		:param key: The key of the list.
		:param index: The index of the item.
		:return: The item at the index.
		"""
		return self.request("LIST_GET", key, index)
	
	def list_set(self, key: str, index: int, value: str):
		"""
		Set an item in a list.
		:param key: The key of the list.
		:param index: The index of the item.
		:param value: The value to set.
		"""
		return self.request("LIST_SET", key, index, value)
	
	def list_push_left(self, key: str, list: list):
		"""
		Push items to the left of a list.
		:param key: The key of the list.
		:param list: The items to push.
		:return: The number of items in the list.
		"""
		return self.request("LIST_PUSH_LEFT", key, list)
	
	def list_push_right(self, key: str, list: list):
		"""
		Push items to the right of a list.
		:param key: The key of the list.
		:param list: The items to push.
		:return: The number of items in the list.
		"""
		return self.request("LIST_PUSH_RIGHT", key, list)
	
	def list_pop_left(self, key: str, count: int = 1):
		"""
		Pop items from the left of a list.
		:param key: The key of the list.
		:param count: The number of items to pop.
		:return: The popped items.
		"""
		return self.request("LIST_POP_LEFT", key, count)
	
	def list_pop_right(self, key: str, count: int = 1):
		"""
		Pop items from the right of a list.
		:param key: The key of the list.
		:param count: The number of items to pop.
		:return: The popped items.
		"""
		return self.request("LIST_POP_RIGHT", key, count)
	
	def list_range(self, key: str, start: int, end: int):
		"""
		Get a range of items from a list.
		:param key: The key of the list.
		:param start: The start index.
		:param end: The end index.
		:return: The items in the range.
		"""
		return self.request("LIST_RANGE", key, start, end)
	
	def list_trim(self, key: str, start: int, end: int):
		"""
		Trim a list.
		:param key: The key of the list.
		:param start: The start index.
		:param end: The end index.
		:return: The number of items trimmed.
		"""
		return self.request("LIST_TRIM", key, start, end)
	
	def list_remove(self, key: str, value: str, count: int = 1):
		"""
		Remove items from a list.
		:param key: The key of the list.
		:param value: The value to remove.
		:param count: The number of items to remove.
		:return: The number of items removed.
		"""
		return self.request("LIST_REMOVE", key, value, count)
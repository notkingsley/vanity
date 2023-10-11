from client.response import Response
from client.socket_client import SocketClient


def format(msg: str | int | float) -> str:
	"""
	Format a message body to be sent to the server.
	Escape the quotes in the message if it is a string
	:param msg: The message to format.
	:return: The formatted message.
	"""
	if isinstance(msg, (int, float)):
		return str(msg)
	return '"' + msg.replace('"', r'\"') + '"'


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
	):
		"""
		Connect to a Vanity Server.
		:param host: The host to connect to.
		:param port: The port to connect to.
		:param no_login: Whether to skip the login process.
		:param username: The username to login with.
		:param password: The password to login with.
		"""
		self.sock = SocketClient(host, port)
		if not no_login:
			self.auth(username, password)
	
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
from client.response import escape, Response
from client.socket_client import SocketClient


class Client:
	"""
	A Client provides a programmatic interface for interacting with a Vanity Server.
	"""
	def __init__(self, host: str, port: int):
		self.sock = SocketClient(host, port)
	
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
		self.sock.send(f"{command} {' '.join(map(escape, args))}")
	
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
	
	def set(self, key: str, value: str):
		"""
		Set the value of a key.
		:param key: The key to set.
		:param value: The value to set.
		"""
		return self.request("SET", key, value)
	
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
from warnings import warn

from client.client_ import Client, AuthLevel
from client.command_interface import CommandInterface


class Pipe(CommandInterface):
	"""
	A Pipe allows sending multiple messages at once to a server.
	"""
	def __init__(self, client: Client):
		self.client = client
		self.requests: list[str] = list()

	def request(self, command: str, *args):
		"""
		Add a request to the pipe.
		"""
		self.requests.append(self.client.request_from(command, *args))

	def execute(self):
		"""
		Execute the requests.
		"""
		request = f"PIPE({len(self.requests)}){''.join(self.requests)}"
		self.requests.clear()
		self.client.send_request(request)
		return self.client.read_response()
	
	def _warn_disallowed(self, command: str):
		warn(f"Command will fail: {command} is not allowed in a pipe", stacklevel= 3)

	def auth(self, username: str, password: str):
		"""
		Authenticate with the server.
		:param username: The username to authenticate with.
		:param password: The password to authenticate with.
		"""
		self._warn_disallowed("AUTH")
		return super().auth(username, password)
	
	def add_user(self, username: str, password: str):
		"""
		Add a user to the server.
		:param username: The username to add.
		:param password: The password to add.
		"""
		self._warn_disallowed("ADD_USER")
		return super().add_user(username, password)
	
	def edit_user(self, username: str, auth_level: AuthLevel):
		"""
		Edit a user's auth level on the server.
		:param username: The username to edit.
		:param auth_level: The new auth level.
		"""
		self._warn_disallowed("EDIT_USER")
		return super().edit_user(username, auth_level)
	
	def del_user(self, username: str):
		"""
		Delete a user from the server.
		:param username: The username to delete.
		"""
		self._warn_disallowed("DEL_USER")
		return super().del_user(username)
	
	def change_password(self, password: str):
		"""
		Change the current client's password.
		:param password: The new password.
		"""
		self._warn_disallowed("CHANGE_PASSWORD")
		return super().change_password(password)
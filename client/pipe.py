from client.client_ import Client
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
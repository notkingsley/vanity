import socket


class SocketClosedError(Exception):
	"""
	An exception thrown when a socket is closed.
	"""
	pass


class SocketClient:
	"""
	A SocketClient mirrors the underlying socket processing method
	of a Vanity Server, allowing us to send and receive messages from
	the server over a socket.
	"""
	DELIM = "~"
	BUFFER_SIZE = 1024

	def __init__(self, host: str, port: int):
		self.sock = socket.create_connection((host, port))
		self.buffer = str()
		self.unprocessed = str()

	def escape(self, msg: str):
		"""
		Escape the message to be sent.
		:param msg: The message to escape.
		:return: The escaped message.
		"""
		return f"{msg.replace(self.DELIM, self.DELIM * 2)}{self.DELIM}\n"

	def send(self, msg: str):
		"""
		Send msg directly to the server.
		:param msg: The message to send.
		"""
		self.sock.sendall(self.escape(msg).encode())

	def recv(self):
		"""
		Read a message into the buffer.
		:return: The message received.
		"""
		data = self.sock.recv(self.BUFFER_SIZE).decode()
		data = self.unprocessed + data
		self.unprocessed = str()

		if not data:
			raise SocketClosedError()

		i = 0
		while i < len(data):
			if data[i] == self.DELIM:
				if i == len(data) - 1:
					# end of data, so we need to wait for more
					self.unprocessed = self.DELIM
					return None

				else:
					if data[i + 1] == self.DELIM:
						# delim was escaped, so we need to keep going
						self.buffer += self.DELIM
						i += 1

					else:
						# delim was not escaped, so we have a full message
						ret = self.buffer
						self.buffer = str()
						self.unprocessed = data[i + 1:]
						return ret

			else:
				self.buffer += data[i]
			i += 1

		return None

	def read_msg(self):
		"""
		Receive a message from the server.
		:return: The message received.
		"""
		while True:
			msg = self.recv()
			if msg is not None:
				return msg.strip()

	def close(self):
		"""
		Close the socket.
		"""
		self.sock.close()

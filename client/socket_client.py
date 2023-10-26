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
	BUFFER_SIZE = 1024

	def __init__(self, host: str, port: int):
		self.sock = socket.create_connection((host, port))
		self.buffer = str()
		self._expected = int()

	def send(self, msg: str):
		"""
		Send msg directly to the server.
		:param msg: The message to send.
		"""
		size = len(msg).to_bytes(4, "big")
		self.sock.sendall(size + msg.encode())

	def read_size(self):
		"""
		Read the size of the next message.
		"""
		size = self.sock.recv(4)
		if not size:
			raise SocketClosedError()
		self._expected = int.from_bytes(size, "big")

	def recv(self):
		"""
		Read a message into the buffer.
		:return: The message received,
		or None if the message is not yet complete.
		"""
		data = self.sock.recv(self.BUFFER_SIZE).decode()
		if not data:
			raise SocketClosedError()
		
		self.buffer += data
		if len(self.buffer) < self._expected:
			return None

		if len(self.buffer) == self._expected:
			ret = self.buffer
			self.buffer = str()
			return ret
		
		else:
			ret = self.buffer[:self._expected]
			self.buffer = self.buffer[self._expected:]
			return ret
	
	def read_msg(self):
		"""
		Receive a message from the server.
		:return: The message received.
		"""
		self.read_size()
		while True:
			msg = self.recv()
			if msg is not None:
				return msg

	def close(self):
		"""
		Close the socket.
		"""
		self.sock.close()


if __name__ == "__main__":
	sock = SocketClient("localhost", 9954)
	sock.send("PING")
	print(sock.read_msg())
	sock.close()
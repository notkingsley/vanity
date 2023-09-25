from enum import Enum


class ServerConstant(Enum):
	"""
	Server response prefixes.
	"""
	OK = "OK"
	NULL = "NULL"
	ERROR = "ERROR"
	PONG = "PONG"


class InvalidResponseError(Exception):
	"""
	An exception thrown when a response is invalid.
	"""
	pass


def parse(msg: str) -> str | int | float:
	"""
	parse a message body received from the server.
	Unecape the quotes in the message if it is a string.
	:param msg: The message to parse.
	:return: The parse message.
	"""
	if msg.startswith('"') and msg.endswith('"'):
		return msg[1:-1].replace(r'\"', '"')
	
	num = float(msg)
	if num.is_integer():
		num = int(num)
	
	return num


class Response:
	def __init__(self, body: str) -> None:
		self._raw = body.strip()
		self._parse()

	def __str__(self) -> str:
		status = self.status.name if self.status else self.status
		body = self.body
		if self.body:
			if len(self.body) > 30:
				body = self.body[:30] + "..."

		return f"<Response: status={status}, body={body}>"

	def _parse(self):
		"""
		Parse the raw response.
		"""
		self.status = None
		self.body = None
		raw = self._raw

		for constant in ServerConstant:
			if raw.startswith(constant.value):
				self.status = constant
				raw = raw[len(constant.value):].strip()
				break
		
		if raw:
			self.body = parse(raw)

		if self.status is None and self.body is None:
			raise InvalidResponseError("Response is empty.")
	
	def is_null(self) -> bool:
		"""
		:return: True if the response is NULL, False otherwise.
		"""
		return self.status == ServerConstant.NULL
	
	def is_error(self) -> bool:
		"""
		:return: True if the response is ERROR, False otherwise.
		"""
		return self.status == ServerConstant.ERROR
	
	def is_ok(self) -> bool:
		"""
		:return: True if the response is OK, False otherwise.
		"""
		return self.status == ServerConstant.OK
	
	def is_pong(self) -> bool:
		"""
		:return: True if the response is PONG, False otherwise.
		"""
		return self.status == ServerConstant.PONG
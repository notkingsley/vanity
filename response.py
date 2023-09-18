from enum import Enum


class ServerConstant(Enum):
	"""
	Server response prefixes.
	"""
	OK = "OK"
	NULL = "NULL"
	ERROR = "ERROR"


class InvalidResponseError(Exception):
	"""
	An exception thrown when a response is invalid.
	"""
	pass

def escape(msg: str):
	"""
	Escape the quotes in the message to be sent.
	:param msg: The message to escape.
	:return: The escaped message.
	"""
	return '"' + msg.replace('"', r'\"') + '"'

def unescape(msg: str) -> str:
	"""
	Unecape the quotes in the message.
	:param msg: The message to unescape.
	:return: The unescaped message.
	"""
	if msg.startswith('"') and msg.endswith('"'):
		return msg[1:-1].replace(r'\"', '"')
	
	raise InvalidResponseError("Message must be quoted.")


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
			self.body = unescape(raw)

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

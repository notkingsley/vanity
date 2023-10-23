from enum import Enum


class ServerConstant(Enum):
	"""
	Server response prefixes.
	"""
	OK = "OK"
	NULL = "NULL"
	ERROR = "ERROR"
	PONG = "PONG"
	DENIED = "DENIED"
	INTERNAL_ERROR = "INTERNAL_ERROR"
	BAD_REQUEST = "BAD_REQUEST"
	BAD_TYPE = "BAD_TYPE"


class InvalidResponseError(Exception):
	"""
	An exception thrown when a response is invalid.
	"""
	pass


def extract_status(msg: str) -> tuple[ServerConstant | None, str]:
	"""
	Extract the status of a response, if any.
	Return the status, and the remaining message.
	"""
	for constant in ServerConstant:
		if msg.startswith(constant.value):
			return constant, msg[len(constant.value):].strip()
	
	return None, msg


def extract_str(msg: str) -> str | None:
	"""
	Extract a string from a response.
	:param msg: The response to extract from.
	:return: The extracted string, or None if no string could be deciphered.
	"""
	if msg.startswith('"') and msg.endswith('"'):
		return msg[1:-1].replace(r'\"', '"')
	
	return None

def extract_as(msg: str, type: type[int | float | str]):
	"""
	Extract a value from a response as a given type.
	:param msg: The response to extract from.
	:param type: The type to extract as.
	:return: The extracted value, or None if no value could be deciphered.
	"""
	if type is str:
		return extract_str(msg)
	
	if type is int or type is float:
		try:
			return type(msg)
		except ValueError:
			return None


def extract_type(msg: str) -> tuple[type[str | int | float] | None, str]:
	"""
	Extract the type of a response, if any.
	Return the type, and the remaining message.
	"""
	if msg.startswith(":STR"):
		return str, msg[4:].strip()
	
	elif msg.startswith(":INT"):
		return int, msg[4:].strip()
	
	elif msg.startswith(":FLOAT"):
		return float, msg[6:].strip()

	return None, msg


class Response:
	def __init__(self, body: str) -> None:
		self._raw = body.strip()

		raw = self._raw
		self.status, raw = extract_status(raw)
		self.type, raw = extract_type(raw)

		self.value = None
		if self.type:
			self.value = extract_as(raw, self.type)
			return

	def __str__(self) -> str:
		status = self.status.name if self.status else self.status
		body = self._raw
		if self._raw and len(self._raw) > 30:
			body = self._raw[:30] + "..."

		return f"<Response: status={status}, body={body}>"

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
	
	def is_denied(self) -> bool:
		"""
		:return: True if the response is DENIED, False otherwise.
		"""
		return self.status == ServerConstant.DENIED
	
	def is_internal_error(self) -> bool:
		"""
		:return: True if the response is INTERNAL_ERROR, False otherwise.
		"""
		return self.status == ServerConstant.INTERNAL_ERROR
	
	def is_bad_request(self) -> bool:
		"""
		:return: True if the response is BAD_REQUEST, False otherwise.
		"""
		return self.status == ServerConstant.BAD_REQUEST
	
	def is_bad_type(self) -> bool:
		"""
		:return: True if the response is BAD_TYPE, False otherwise.
		"""
		return self.status == ServerConstant.BAD_TYPE
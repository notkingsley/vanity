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


class InvalidResponseError(Exception):
	"""
	An exception thrown when a response is invalid.
	"""
	pass


def extract(msg: str) -> str | int | float | None:
	"""
	Extract a value from a response.
	:param msg: The response to extract from.
	:return: The extracted value, or None if no value could be deciphered.
	"""
	if msg.startswith(":"):
		msg = msg[1:].strip()
		
		if msg.startswith("STR"):
			msg = msg[3:].strip()
			if msg.startswith('"') and msg.endswith('"'):
				return msg[1:-1].replace(r'\"', '"')
		
		elif msg.startswith("INT"):
			msg = msg[3:].strip()
			if msg.isdigit():
				return int(msg)
		
		elif msg.startswith("FLOAT"):
			msg = msg[5:].strip()
			if msg.replace(".", "", 1).isdigit():
				return float(msg)
	
	return None

class Response:
	def __init__(self, body: str) -> None:
		self._raw = body.strip()
		self._parse()

	def __str__(self) -> str:
		status = self.status.name if self.status else self.status
		body = self.body
		if self.body and len(self.body) > 30:
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
			self.body = raw
			self.value = extract(self.body)

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
	
	def is_denied(self) -> bool:
		"""
		:return: True if the response is DENIED, False otherwise.
		"""
		return self.status == ServerConstant.DENIED
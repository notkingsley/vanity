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


class InvalidResponse(Exception):
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
			return constant, msg[len(constant.value):].lstrip()
	
	return None, msg


def extract_len(msg: str):
	"""
	Extract a length from a response.
	:param msg: The response to extract from.
	:return: The extracted length, or None if no length could be deciphered.
	"""
	if msg.startswith("("):
		num, msg = msg[1:].split(")", 1)
		try:
			return int(num), msg
		except ValueError:
			raise InvalidResponse(f"Could not extract length from {num}.")
	
	return None, msg

def extract_str(msg: str) -> tuple[str, str]:
	"""
	Extract a string from a response.
	:param msg: The response to extract from.
	:return: The extracted string, or None if no int could be deciphered.
	"""
	num, msg = extract_len(msg)
	if num is None:
		return None, msg
	return msg[:num], msg[num:]


def extract_int(msg: str) -> tuple[int | None, str]:
	"""
	Extract an int from a response.
	:param msg: The response to extract from.
	:return: The extracted int, or None if no int could be deciphered.
	"""
	num = str()
	for char in msg:
		if char.isdigit() or char == "-":
			num += char
		else:
			break

	if not num:
		return None, msg
	try:
		return int(num), msg[len(num):].lstrip()
	except ValueError:
		return None, msg


def extract_float(msg: str):
	"""
	Extract a float from a response.
	:param msg: The response to extract from.
	:return: The extracted float, or None if no float could be deciphered.
	"""
	num = str()
	for char in msg:
		if char.isdigit() or char == "." or char == "-":
			num += char
		else:
			break

	if not num:
		return None, msg
	try:
		return float(num), msg[len(num):].lstrip()
	except ValueError:
		return None, msg


def extract_list(msg: str):
	"""
	Extract a list from a response.
	:param msg: The response to extract from.
	:return: The extracted list, or None if no list could be deciphered.
	"""
	num, msg = extract_len(msg)
	if num is None:
		return None, msg
	
	if not msg.startswith("["):
		return None, msg
	msg = msg[1:].lstrip()
	
	elements = list()
	for _ in range(num):
		_type, msg = extract_type(msg)
		if _type is None:
			raise InvalidResponse(f"Could not extract type from {msg}.")
		
		element, msg = extract_as(msg, _type)
		if element is None and _type is not type(None):
			raise InvalidResponse(f"Could not extract element from {msg}.")
		
		elements.append(element)

	if not msg.startswith("]"):
		raise InvalidResponse(f"Message did not end with ']'.")
	msg = msg[1:].lstrip()
	
	return elements, msg


def extract_as(msg: str, _type: type[int | float | str | list | None]):
	"""
	Extract a value from a response as a given type.
	:param msg: The response to extract from.
	:param type: The type to extract as.
	:return: The extracted value, or None if no value could be deciphered.
	"""
	if _type is str:
		return extract_str(msg)
	if _type is int:
		return extract_int(msg)
	if _type is float:
		return extract_float(msg)
	if _type is list:
		return extract_list(msg)
	if _type is type(None):
		return None, msg


def extract_type(msg: str) -> tuple[type[str | int | float | list | None] | None, str]:
	"""
	Extract the type of a response, if any.
	Return the type, and the remaining message.
	"""
	if msg.startswith(":STR"):
		return str, msg[4:].lstrip()
	
	elif msg.startswith(":INT"):
		return int, msg[4:].lstrip()
	
	elif msg.startswith(":FLOAT"):
		return float, msg[6:].lstrip()
	
	elif msg.startswith(":NULL"):
		return type(None), msg[5:].lstrip()
	
	elif msg.startswith(":ARR"):
		return list, msg[4:].lstrip()

	return None, msg


class Response:
	def __init__(self, body: str) -> None:
		self._raw = body

		raw = self._raw
		self.status, raw = extract_status(raw)
		self.type, raw = extract_type(raw)

		self.value = None
		if self.type:
			self.value, raw = extract_as(raw, self.type)
			if raw:
				print(f"Raw: {raw}")
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
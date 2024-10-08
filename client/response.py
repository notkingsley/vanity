from dataclasses import dataclass
from enum import Enum
from warnings import warn

from client.auth_level import AuthLevel


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
    BAD_STATE = "BAD_STATE"
    QUEUED = "QUEUED"
    ASYNC = "ASYNC"


class ServerType(Enum):
    """
    Server response types.
    """

    STRING = "STR"
    INT = "INT"
    FLOAT = "FLOAT"
    ARRAY = "ARR"
    TUPLE = "TUPLE"
    LIST = "LIST"
    NULL = "NULL"
    SET = "SET"
    BOOL = "BOOL"
    HASH = "HASH"
    AGG = "AGG"
    AUTH_LEVEL = "AUTH_LEVEL"
    ERR = "ERR"


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
            legnth = len(constant.value)
            return constant, msg[legnth:].lstrip()

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
    :return: The extracted string, or None if no string could be deciphered.
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
        length = len(num)
        return int(num), msg[length:].lstrip()
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
        length = len(num)
        return float(num), msg[length:].lstrip()
    except ValueError:
        return None, msg


def extract_array(msg: str):
    """
    Extract an array from a response.
    :param msg: The response to extract from.
    :return: The extracted array, or None if no array could be deciphered.
    """
    num, msg = extract_len(msg)
    if num is None:
        return None, msg

    if not msg.startswith("["):
        return None, msg
    msg = msg[1:].lstrip()

    elements = list()
    for _ in range(num):
        element, msg = extract_as(msg, ServerType.STRING)
        elements.append(element)

    if not msg.startswith("]"):
        raise InvalidResponse("Message did not end with ']'.")
    msg = msg[1:].lstrip()

    return elements, msg


def extract_tuple(msg: str):
    """
    Extract a tuple from a response.
    :param msg: The response to extract from.
    :return: The extracted tuple (actually a list), or None if no tuple could be deciphered.
    """
    num, msg = extract_len(msg)
    if num is None:
        return None, msg

    if not msg.startswith("("):
        return None, msg
    msg = msg[1:].lstrip()

    elements = list()
    for _ in range(num):
        _type, msg = extract_type(msg)
        if _type is None:
            raise InvalidResponse(f"Could not extract type from {msg}.")

        element, msg = extract_as(msg, _type)
        if element is None and _type is not ServerType.NULL:
            raise InvalidResponse(f"Could not extract element from {msg}.")

        elements.append(element)

    if not msg.startswith(")"):
        raise InvalidResponse("Message did not end with ')'.")
    msg = msg[1:].lstrip()

    return elements, msg


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
        element, msg = extract_as(msg, ServerType.STRING)
        elements.append(element)

    if not msg.startswith("]"):
        raise InvalidResponse("Message did not end with ']'.")
    msg = msg[1:].lstrip()

    return elements, msg


def extract_set(msg: str):
    """
    Extract a set from a response.
    :param msg: The response to extract from.
    :return: The extracted set, or None if no set could be deciphered.
    """
    num, msg = extract_len(msg)
    if num is None:
        return None, msg

    if not msg.startswith("{"):
        return None, msg
    msg = msg[1:].lstrip()

    elements = set()
    for _ in range(num):
        element, msg = extract_as(msg, ServerType.STRING)
        elements.add(element)

    if not msg.startswith("}"):
        raise InvalidResponse("Message did not end with '{'.")
    msg = msg[1:].lstrip()

    return elements, msg


def extract_bool(msg: str):
    """
    Extract a bool from a response.
    :param msg: The response to extract from.
    :return: The extracted bool, or None if no bool could be deciphered.
    """
    if msg.startswith("true"):
        return True, msg[4:].lstrip()
    if msg.startswith("false"):
        return False, msg[5:].lstrip()
    return None, msg


def extract_hash(msg: str):
    """
    Extract a hash from a response.
    :param msg: The response to extract from.
    :return: The extracted hash, or None if no hash could be deciphered.
    """
    num, msg = extract_len(msg)
    if num is None:
        return None, msg

    if not msg.startswith("{"):
        return None, msg
    msg = msg[1:].lstrip()

    elements = dict()
    for _ in range(num):
        key, msg = extract_as(msg, ServerType.STRING)
        value, msg = extract_as(msg, ServerType.STRING)
        elements[key] = value

    if not msg.startswith("}"):
        raise InvalidResponse("Message did not end with '}'.")
    msg = msg[1:].lstrip()

    return elements, msg


def extract_agg(msg: str):
    """
    Extract an aggregate response.
    :param msg: The response to extract from.
    :return: The extracted list of responses, or None if no aggregate could be deciphered.
    """
    num, msg = extract_len(msg)
    if num is None:
        return None, msg

    responses: list[Response] = list()
    for _ in range(num):
        response, msg = extract_response(msg)
        responses.append(Response(response))

    return responses, msg


def extract_auth_level(msg: str):
    """
    Extract an auth level from a response.
    :param msg: The response to extract from.
    :return: The extracted auth level, or None if no auth level could be deciphered.
    """
    for level in AuthLevel:
        if msg.startswith(level.value):
            length = len(level.value)
            return level, msg[length:].lstrip()

    return None, msg


def extract_err(msg: str):
    """
    Extract an ERR from a response
    :param msg: The response to extract from.
    :return: The extracted auth level, or None if no error string could be extracted
    """
    return extract_str(msg)


def extract_as(msg: str, _type: ServerType):
    """
    Extract a value from a response as a given type.
    :param msg: The response to extract from.
    :param type: The type to extract as.
    :return: The extracted value, or None if no value could be deciphered.
    """
    match _type:
        case ServerType.STRING:
            return extract_str(msg)
        case ServerType.INT:
            return extract_int(msg)
        case ServerType.FLOAT:
            return extract_float(msg)
        case ServerType.ARRAY:
            return extract_array(msg)
        case ServerType.TUPLE:
            return extract_tuple(msg)
        case ServerType.LIST:
            return extract_list(msg)
        case ServerType.NULL:
            return None, msg
        case ServerType.SET:
            return extract_set(msg)
        case ServerType.BOOL:
            return extract_bool(msg)
        case ServerType.HASH:
            return extract_hash(msg)
        case ServerType.AGG:
            return extract_agg(msg)
        case ServerType.AUTH_LEVEL:
            return extract_auth_level(msg)
        case ServerType.ERR:
            return extract_err(msg)
        case _:
            raise ValueError(f"Invalid type: {_type}.")


def extract_type(msg: str) -> tuple[ServerType | None, str]:
    """
    Extract the type of a response, if any.
    Return the type, and the remaining message.
    """
    if msg.startswith(":STR"):
        return ServerType.STRING, msg[4:].lstrip()

    elif msg.startswith(":INT"):
        return ServerType.INT, msg[4:].lstrip()

    elif msg.startswith(":FLOAT"):
        return ServerType.FLOAT, msg[6:].lstrip()

    elif msg.startswith(":NULL"):
        return ServerType.NULL, msg[5:].lstrip()

    elif msg.startswith(":ARR"):
        return ServerType.ARRAY, msg[4:].lstrip()

    elif msg.startswith(":TUPLE"):
        return ServerType.TUPLE, msg[6:].lstrip()

    elif msg.startswith(":LIST"):
        return ServerType.LIST, msg[5:].lstrip()

    elif msg.startswith(":SET"):
        return ServerType.SET, msg[4:].lstrip()

    elif msg.startswith(":BOOL"):
        return ServerType.BOOL, msg[5:].lstrip()

    elif msg.startswith(":HASH"):
        return ServerType.HASH, msg[5:].lstrip()

    elif msg.startswith(":AGG"):
        return ServerType.AGG, msg[4:].lstrip()

    elif msg.startswith(":AUTH_LEVEL"):
        return ServerType.AUTH_LEVEL, msg[11:].lstrip()

    elif msg.startswith(":ERR"):
        return ServerType.ERR, msg[4:].lstrip()

    return None, msg


def extract_response(raw: str):
    """
    Convert a raw response to a Response.
    :param raw: The raw response string.
    :return: The RawResponse and the remaining message.
    """
    status, raw = extract_status(raw)
    if status is None:
        raise InvalidResponse("No status could be extracted.")

    type, raw = extract_type(raw)
    if type is not None:
        value, raw = extract_as(raw, type)
    else:
        value = None

    return RawResponse(status, type, value), raw


@dataclass
class RawResponse:
    """
    Represents a raw response just after extraction.
    """

    ValueType = (
        str
        | int
        | float
        | list
        | set
        | dict
        | bool
        | list["Response"]
        | AuthLevel
        | None
    )

    status: ServerConstant
    type: ServerType | None
    value: ValueType


class Response:
    def __init__(self, raw: str | RawResponse) -> None:
        rem = None
        if isinstance(raw, str):
            raw, rem = extract_response(raw)
            self._warn_trailing(rem)

        self.status = raw.status
        self.type = raw.type
        self.value = raw.value

    def __str__(self) -> str:
        status = self.status.name if self.status else self.status
        status = f"status={status}" if status else ""

        type = self.type.name if self.type else self.type
        type = f"type={type}" if type else ""

        err = ""
        if self.type_is_err():
            err = self.value
            if len(err) > 30:
                err = err[:30] + "..."
        err = f"err={err}" if err else ""

        ret = f"<{self.__class__.__name__}"
        if any((status, type, err)):
            ret += f": {', '.join(filter(None, (status, type, err)))}"
        ret += ">"
        return ret

    def __repr__(self) -> str:
        return str(self)

    def _warn_trailing(self, rem: str) -> None:
        if rem:
            warn(f"Response has trailing data: {rem}", stacklevel=3)

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

    def is_bad_state(self) -> bool:
        """
        :return: True if the response is BAD_STATE, False otherwise.
        """
        return self.status == ServerConstant.BAD_STATE

    def is_queued(self) -> bool:
        """
        :return: True if the response is QUEUED, False otherwise.
        """
        return self.status == ServerConstant.QUEUED

    def is_async(self) -> bool:
        """
        :return: True if the response is ASYNC, False otherwise.
        """
        return self.status == ServerConstant.ASYNC

    def type_is_str(self) -> bool:
        """
        :return: True if the response is of type STRING, False otherwise.
        """
        return self.type == ServerType.STRING

    def type_is_int(self) -> bool:
        """
        :return: True if the response is of type INT, False otherwise.
        """
        return self.type == ServerType.INT

    def type_is_float(self) -> bool:
        """
        :return: True if the response is of type FLOAT, False otherwise.
        """
        return self.type == ServerType.FLOAT

    def type_is_array(self) -> bool:
        """
        :return: True if the response is of type ARRAY, False otherwise.
        """
        return self.type == ServerType.ARRAY

    def type_is_tuple(self) -> bool:
        """
        :return: True if the response is of type TUPLE, False otherwise.
        """
        return self.type == ServerType.TUPLE

    def type_is_null(self) -> bool:
        """
        :return: True if the response is of type NULL, False otherwise.
        """
        return self.type == ServerType.NULL

    def type_is_list(self) -> bool:
        """
        :return: True if the response is of type LIST, False otherwise.
        """
        return self.type == ServerType.LIST

    def type_is_set(self) -> bool:
        """
        :return: True if the response is of type SET, False otherwise.
        """
        return self.type == ServerType.SET

    def type_is_bool(self) -> bool:
        """
        :return: True if the response is of type BOOL, False otherwise.
        """
        return self.type == ServerType.BOOL

    def type_is_hash(self) -> bool:
        """
        :return: True if the response is of type HASH, False otherwise.
        """
        return self.type == ServerType.HASH

    def type_is_agg(self) -> bool:
        """
        :return: True if the response is an aggregate, False otherwise.
        """
        return self.type == ServerType.AGG

    def type_is_auth_level(self) -> bool:
        """
        :return: True if the response is of type AUTH_LEVEL, False otherwise.
        """
        return self.type == ServerType.AUTH_LEVEL

    def type_is_err(self) -> bool:
        """
        :return: True if the response is of type ERR, False otherwise.
        """
        return self.type == ServerType.ERR

from enum import Enum

from client.response import (
    extract_status,
    extract_str,
    InvalidResponse,
    Response,
    ServerConstant,
)


class AsyncType(Enum):
    """
    The type of asynchronous data.
    """

    PUBLISH = "PUBLISH"


class AsyncData:
    """
    Data sent asynchronously.
    """

    def is_publish(self) -> bool:
        """
        :return: True if the data is a PublishData, False otherwise.
        """
        return isinstance(self, PublishData)


class PublishData(AsyncData):
    """
    Represents data published to a channel.
    """

    def __init__(self, channel: str, message: str) -> None:
        """
        Create a new PublishData.
        :param channel: The channel the message was published to.
        :param message: The message that was published.
        """
        self.channel = channel
        self.message = message


def extract_async_type(msg: str) -> tuple[AsyncType, str]:
    """
    Extract an asynchronous data type.
    :param msg: The response to extract from.
    :return: The extracted AsyncType.
    """
    if msg.startswith(":PUBLISH"):
        return AsyncType.PUBLISH, msg[8:].lstrip()
    
    raise InvalidResponse(f"Invalid async type: {msg}.")


def extract_publish(msg: str) -> tuple[PublishData, str]:
    """
    Extract a publish response.
    :param msg: The response to extract from.
    :return: The extracted PublishData.
    """
    channel, msg = extract_str(msg)
    message, msg = extract_str(msg)
    return PublishData(channel, message), msg


def extract_async(msg: str) -> tuple[AsyncData, str]:
    """
    Extract an asynchronous response.
    :param msg: The response to extract from.
    :return: The extracted AsyncData.
    """
    status, msg = extract_status(msg)
    if status != ServerConstant.ASYNC:
        raise InvalidResponse(f"Invalid async status: {status}.")

    data_type, msg = extract_async_type(msg)
    match data_type:
        case AsyncType.PUBLISH:
            return extract_publish(msg)
        case _:
            raise InvalidResponse(f"Invalid async data type: {data_type}.")


class AsyncResponse(Response):
    """
    Represents an asynchronous response.
    """

    def __init__(self, raw: str) -> None:
        """
        Create a new AsyncResponse.
        """
        self.data, raw = extract_async(raw)
        self._warn_trailing(raw)
        self.status = ServerConstant.ASYNC
        self.type = None
        self.value = None
        self.body = None


def make_response(message: str) -> AsyncResponse | Response:
    """
    Convert a raw response to a Response.
    :param message: The raw response string.
    :return: The converted Response.
    """
    status, _ = extract_status(message)
    if status == ServerConstant.ASYNC:
        return AsyncResponse(message)
    return Response(message)

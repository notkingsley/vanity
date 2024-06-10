from collections import deque
from itertools import chain

from client.async_response import AsyncResponse, make_response
from client.auth_level import AuthLevel
from client.response import Response
from client.socket_client import SocketClient
from client.command_interface import CommandInterface


def format(msg: str | int | float | list | set | dict | AuthLevel) -> str:
    """
    Format a message body to be sent to the server.
    Escape the quotes in the message if it is a string
    :param msg: The message to format.
    :return: The formatted message.
    """
    if isinstance(msg, (int, float)):
        return str(msg)
    elif isinstance(msg, AuthLevel):
        return msg.value
    elif isinstance(msg, list):
        return f"({len(msg)})[" + "".join(map(format, msg)) + "]"
    elif isinstance(msg, str):
        return f"({len(msg)})" + msg
    elif isinstance(msg, set):
        return f"({len(msg)})" + "{" + "".join(map(format, msg)) + "}"
    elif isinstance(msg, dict):
        iterable = chain.from_iterable((key, value) for key, value in msg.items())
        return f"({len(msg)})" + "{" + "".join(map(format, iterable)) + "}"
    else:
        raise TypeError(f"Cannot format {msg}.")


class Client(CommandInterface):
    """
    A Client provides a programmatic interface for interacting with a Vanity Server.
    """

    DEFAULT_ADMIN_USERNAME = "vanity"
    DEFAULT_ADMIN_PASSWORD = "vanity"

    def __init__(
        self,
        host: str,
        port: int,
        *,
        no_login: bool = False,
        username: str | None = None,
        password: str | None = None,
        db_index: int | None = None,
    ):
        """
        Connect to a Vanity Server.
        :param host: The host to connect to.
        :param port: The port to connect to.
        :param no_login: Whether to skip the login process.
        :param username: The username to login with.
        :param password: The password to login with.
        :param db_index: The database index to switch to.
        """
        self.sock = SocketClient(host, port)
        self.sync_responses: deque[Response] = deque()
        self.async_responses: deque[AsyncResponse] = deque()

        if not no_login:
            username = username or self.DEFAULT_ADMIN_USERNAME
            password = password or self.DEFAULT_ADMIN_PASSWORD
            self.auth(username, password)

        if db_index is not None:
            self.switch_db(db_index)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    @staticmethod
    def request_from(command: str, *args) -> str:
        """
        Format a request to be sent to the server.
        :param command: The command to send.
        :param args: The arguments to send.
        :return: The formatted request.
        """
        return f"{command}{' '.join(map(format, args))}"

    def close(self):
        """
        Close the connection to the server.
        """
        self.sock.close()

    def send_request(self, request: str):
        """
        Send a request to the server.
        :param request: The request to send.
        """
        self.sock.send(request)

    def send_command(self, command: str, *args):
        """
        Send a command to the server.
        :param command: The command to send.
        :param args: The arguments to send.
        """
        self.send_request(self.request_from(command, *args))

    def _read_next_response(self):
        """
        Read a response into one of the response queues.
        """
        response = make_response(self.sock.read_msg())
        if isinstance(response, AsyncResponse):
            self.async_responses.append(response)
        else:
            self.sync_responses.append(response)

    def read_response(self) -> Response:
        """
        Read a synchronous response from the server.
        :return: The response from the server.
        """
        while not self.sync_responses:
            self._read_next_response()

        return self.sync_responses.popleft()

    def request(self, command: str, *args) -> Response:
        """
        Send a request to the server.
        :param command: The command to send.
        :param args: The arguments to send.
        :return: The response from the server.
        """
        self.send_command(command, *args)
        return self.read_response()

    def read_async(self, *, block: bool = True) -> AsyncResponse | None:
        """
        Read an asynchronous response from the server.
        :param block: Whether to block until a response is available.
        :return: The response from the server,
        or None if block is False and no response is available.
        """
        while not self.async_responses:
            if not block:
                return None

            self._read_next_response()

        return self.async_responses.popleft()

    def exit(self):
        """
        Request to exit the session
        """
        self.send_command("EXIT")

    def terminate(self):
        """
        Terminate the server.
        """
        self.send_command("TERMINATE")

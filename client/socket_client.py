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

    def __init__(self, host: str, port: int):
        self.sock = socket.create_connection((host, port))
        self.buffer = str()

    def send(self, msg: str):
        """
        Send msg directly to the server.
        :param msg: The message to send.
        """
        size = len(msg).to_bytes(4, "big")
        self.sock.sendall(size + msg.encode())

    def read_size(self):
        """
        Read and return the size of the next message
        """
        size = self.sock.recv(4)
        if not size:
            raise SocketClosedError()

        return int.from_bytes(size, "big")

    def recv(self, expected: int):
        """
        Read a message into the buffer.
        :param expected: The expected size of the message.
        :return: The message received,
        or None if the message is not yet complete.
        """
        read_size = expected - len(self.buffer)
        assert read_size > 0

        data = self.sock.recv(read_size).decode()
        if not data:
            raise SocketClosedError()

        self.buffer += data
        if len(self.buffer) != expected:
            return None

        ret, self.buffer = self.buffer, str()
        return ret

    def read_msg(self):
        """
        Receive a message from the server.
        :return: The message received.
        """
        size = self.read_size()
        while True:
            msg = self.recv(size)
            if msg is not None:
                return msg

    def close(self):
        """
        Close the socket.
        """
        self.sock.close()

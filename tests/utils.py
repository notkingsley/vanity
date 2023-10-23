import socket

from client.client import Client


def get_free_port():
	"""
	Get a (likely) free port number.
	"""
	s = socket.socket()
	s.bind(('', 0))
	port = s.getsockname()[1]
	s.close()
	return port


def make_client(port, **kwargs) -> Client:
	"""
	Make a client.
	"""
	return Client("localhost", port, **kwargs)
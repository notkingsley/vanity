import sys

from socket_client import SocketClient, SocketClosedError


def cli_loop(host: str, port: int):
	"""
	Run as a command line interface.
	"""
	try:
		sock = SocketClient(host, port)
	except Exception as e:
		print(f"{e.__class__.__name__}: {e}")
		return

	print("Welcome to Vanity!\nType \"exit\" to exit.")
	try:
		while True:
			msg = input(f"vanity@{host}:{port}> ")
			if msg == "exit":
				break
			sock.send(msg)
			print(sock.read_msg())

	except SocketClosedError:
		print("Connection closed.")

	except KeyboardInterrupt:
		print("Exiting.")

	finally:
		sock.close()


if __name__ == "__main__":
	if len(sys.argv) == 3:
		cli_loop(sys.argv[1], int(sys.argv[2]))
	elif len(sys.argv) == 2:
		cli_loop("localhost", int(sys.argv[1]))
	else:
		print("Usage: python client.py [host] port")
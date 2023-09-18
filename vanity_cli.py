import sys

from client import SocketClient, SocketClosedError


def cli_loop(host: str, port: int):
	"""
	Run as a command line interface.
	"""
	print("Welcome to Vanity!\nType \"exit\" to exit.")
	sock = SocketClient(host, port)

	try:
		while True:
			msg = input(f"{host}:{port}> ")
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
import sys

from socket_client import SocketClient, SocketClosedError


def get_default_auth():
	"""
	Get the default authentication string.
	"""
	return """AUTH (6)vanity (6)vanity"""


def get_prompt(host: str, port: int) -> str:
	"""
	Get the prompt for the command line interface.
	"""
	if len(host) > 15:
		host = host[:12] + "..."
	return f"vanity@{host}:{port}> "

def cli_loop(host: str, port: int, no_auth: bool = False):
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
		if not no_auth:
			sock.send(get_default_auth())
			sock.read_msg()

		prompt = get_prompt(host, port)
		while True:
			msg = input(prompt)
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
	if len(sys.argv) == 4:
		if sys.argv[3] != "no-auth":
			print("Usage: python client.py [host] port [no-auth]")
			sys.exit(1)
		cli_loop(sys.argv[1], int(sys.argv[2]), True)
	if len(sys.argv) == 3:
		if sys.argv[2] == "no-auth":
			cli_loop("localhost", int(sys.argv[1]), True)
		else:
			cli_loop(sys.argv[1], int(sys.argv[2]))
	elif len(sys.argv) == 2:
		cli_loop("localhost", int(sys.argv[1]))
	else:
		print("Usage: python client.py [host] port [no-auth]")
		sys.exit(1)
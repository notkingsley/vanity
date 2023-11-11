from abc import ABC, abstractmethod
from time import sleep, time
import socket
from threading import Event, Thread

from client.client import Client
from client.server_handle import ServerHandle


def get_free_port():
	"""
	Get a (likely) free port number.
	"""
	s = socket.socket()
	s.bind(('', 0))
	port = s.getsockname()[1]
	s.close()
	return port


def client_from(server_handle: ServerHandle) -> Client:
	"""
	Create a client from a server handle.
	"""
	return Client("localhost", server_handle.port)


class Load(ABC):
	"""
	Allows to impose a load on a server.
	"""
	def __init__(self, concurrency: int, delay_per_unit: float) -> None:
		self.concurrency = concurrency
		self.delay_per_unit = delay_per_unit
	
	def impose(self, srv: ServerHandle, exit: Event):
		"""
		Impose the load on the srv.
		"""
		threads: list[Thread] = []
		for _ in range(self.concurrency):
			thread = Thread(target= self.do, args= (srv, exit))
			thread.start()
			threads.append(thread)
		return threads

	def do(self, srv: ServerHandle, exit: Event):
		"""
		Do the load.
		"""
		with client_from(srv) as client:
			self.loop(client, exit)
	
	def loop(self, client: Client, exit: Event):
			while not exit.is_set():
				start = time()
				self.action(client)
				delta = time() - start
				if delta < self.delay_per_unit:
					sleep(self.delay_per_unit - delta)
	
	@abstractmethod
	def action(self, client: Client):
		"""
		Do the action.
		"""
		raise NotImplementedError()


class Impose:
	"""
	Impose load on a server.
	"""
	def __init__(self, load: list[Load], srv: ServerHandle) -> None:
		self.load = load
		self.srv = srv
		self.threads: list[Thread] = list()
		self.exit = Event()
	
	def __enter__(self):
		for load in self.load:
			self.threads += load.impose(self.srv, self.exit)
	
	def __exit__(self, exc_type, exc_val, exc_tb):
		self.exit.set()
		for thread in self.threads:
			thread.join()


class Bench(ABC):
	"""
	Bench a command.
	"""
	def run(self, load: list[Load], repeat: int):
		"""
		Run the command.
		"""
		with ServerHandle() as srv:
			with Impose(load, srv):
				with client_from(srv) as client:
					return self.bench(client, repeat)
				
	def bench(self, client: Client, repeat: int):
		"""
		Bench the command.
		:param client: The client to use.
		:param repeat: The number of times to repeat the command.
		:return: The average duration of the command.
		"""
		total = 0
		for _ in range(repeat):
			start = time()
			self.action(client)
			end = time()
			total += end - start
		
		return total / repeat
	
	@abstractmethod
	def action(self, client: Client):
		"""
		Do the action.
		"""
		raise NotImplementedError()


class PingLoad(Load):
	"""
	Allows to impose a ping load on a server.
	"""
	def action(self, client: Client):
		client.ping()


class SetLoad(Load):
	"""
	Allows to impose a set load on a server.
	"""
	def action(self, client: Client):
		client.set("test_set", "test_set_value")


class GetLoad(Load):
	"""
	Allows to impose a get load on a server.
	"""
	def loop(self, client: Client, exit: Event):
		client.set("test_set", "test_set_value")
		super().loop(client, exit)

	def action(self, client: Client):
		client.get("test_set")


class PingBench(Bench):
	"""
	Bench the ping command.
	"""
	def action(self, client: Client):
		client.ping()


class SetBench(Bench):
	"""
	Bench the set command.
	"""
	def action(self, client: Client):
		client.set("test_set", "test_set_value")


class GetBench(Bench):
	"""
	Bench the get command.
	"""
	def bench(self, client: Client, repeat: int):
		client.set("test_set", "test_set_value")
		return super().bench(client, repeat)
	
	def action(self, client: Client):
		client.get("test_set")


def main():
	"""
	Run the benchmark.
	"""
	concurrency = 100
	delay_per_unit = 0.01
	repeat = 100
	
	ping_load = PingLoad(concurrency, delay_per_unit)
	set_load = SetLoad(concurrency, delay_per_unit)
	get_load = GetLoad(concurrency, delay_per_unit)
	load = [ping_load, set_load, get_load]

	for bench_type in [PingBench, SetBench, GetBench]:
		bench: Bench = bench_type()
		avg = bench.run(load, repeat)
		print(f"{bench.__class__.__name__}, avg: {avg}")


if __name__ == "__main__":
	main()
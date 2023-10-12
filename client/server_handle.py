import subprocess
from time import sleep
from typing import Literal


class ServerHandle:
	"""
	A ServerHandle starts a server instance and can be used to stop it.
	"""
	EXECUTABLE_PATH = "vanity/cmake-build-debug/vanity"
	DEFAULT_PORT = 9955
	STARTUP_DELAY = 0.01

	def __init__(
		self,
		*,
		port: int | None = DEFAULT_PORT,
		executable_path: str = EXECUTABLE_PATH,
		no_persist: bool = True,
		persist_file: str = None,
		persist_to_cwd: bool = False,
		log_file: str = None,
		log_level: Literal["debug", "info", "warning", "error", "critical"] = None,
		disable_logging: bool = True,
	):
		"""
		Create a new ServerHandle.
		:param port: The port to run the server on, use server default if None
		:param executable_path: The path to the server executable.
		:param no_persist: Whether to disable persistence.
		:param persist_file: The file to persist to if no_persist is False.
		:param persist_to_cwd: if present and persist_file is None, persists to the 
		current working directory of the executable instead of user's home directory.
		:param log_file: The file to log to.
		:param log_level: The level to log at.
		:param disable_logging: Whether to disable logging.
		"""
		self.args = [executable_path]
		if port is not None:
			self.args.append(f"--port={port}")

		if no_persist:
			self.args.append(f"--no-persist")
		else:
			if persist_file:
				self.args.append(f"--persist-file={persist_file}")
			elif persist_to_cwd:
				self.args.append(f"--persist-to-cwd")
		
		if log_file:
			self.args.append(f"--log-file={log_file}")
		
		if log_level:
			self.args.append(f"--log-level={log_level}")
		
		if disable_logging:
			self.args.append(f"--disable-logging")

	def __enter__(self):
		self.start()
		return self
	

	def __exit__(self, exc_type, exc_val, exc_tb):
		self.stop()

	
	def start(self):
		"""
		Start the server.
		"""
		self.process = subprocess.Popen(self.args)
		sleep(self.STARTUP_DELAY)
	

	def stop(self):
		"""
		Stop the server.
		"""
		self.process.terminate()
		self.process.wait()
		self.process = None

	def restart(self):
		"""
		Stop the instance and start another with the same arguments
		"""
		self.stop()
		self.start()
	
	def is_running(self) -> bool:
		"""
		Whether the server is running.
		:return: True if the server is running, False otherwise.
		"""
		return self.process is not None and self.process.poll() is None
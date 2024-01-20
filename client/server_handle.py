import subprocess
from time import sleep
from typing import Literal


class ServerHandle:
    """
    A ServerHandle starts a server instance and can be used to stop it.
    """

    EXECUTABLE_PATH = "vanity/cmake-build-debug/vanity"
    STARTUP_DELAY = 0.01

    def __init__(
        self,
        *,
        port: int | None = None,
        ports: list[int] | None = None,
        executable_path: str = EXECUTABLE_PATH,
        no_db_persist: bool = True,
        no_users_persist: bool = True,
        persist_file: str = None,
        use_cwd: bool = False,
        log_file: str = None,
        log_level: Literal["debug", "info", "warning", "error", "critical"] = None,
        no_logging: bool = True,
        users_file: str = None,
        env: dict[str, str] = None,
    ):
        """
        Create a new ServerHandle.
        :param port: The port to run the server on (any or both of port and ports can be specified)
        :param ports: Extra ports to run the server on (any or both of port and ports can be specified)
        :param executable_path: The path to the server executable.
        :param no_db_persist: Whether to persist the database.
        :param no_users_persist: Whether to persist the users file.
        :param persist_file: The file to persist the database to if no_db_persist is False.
        :param use_cwd: if True, persist the db and users file (if not absolute) to the
        current working directory of the executable instead of user's home directory.
        :param log_file: The file to log to.
        :param log_level: The level to log at.
        :param users_file: The file to store user's login info in if no_users_persist is False.
        :param env: The environment variables to run the server with.
        """
        self.args = [executable_path]
        self.env = env

        _ports = set()
        if port is not None:
            _ports.add(port)

        if ports is not None:
            _ports.update(ports)

        if _ports:
            for port in _ports:
                self.args.append(f"--port={port}")

        if use_cwd:
            self.args.append(f"--use-cwd")

        if no_db_persist:
            self.args.append(f"--no-db-persist")
        else:
            if persist_file:
                self.args.append(f"--persist-file={persist_file}")

        if no_users_persist:
            self.args.append(f"--no-users-persist")
        else:
            if users_file:
                self.args.append(f"--users-file={users_file}")

        if log_file:
            self.args.append(f"--log-file={log_file}")

        if log_level:
            self.args.append(f"--log-level={log_level}")

        if no_logging:
            self.args.append(f"--no-logging")

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop()

    def start(self):
        """
        Start the server.
        """
        self.process = subprocess.Popen(self.args, env=self.env)
        sleep(self.STARTUP_DELAY)

    def stop(self):
        """
        Stop the server.
        """
        self.process.kill()
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

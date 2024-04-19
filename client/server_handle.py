import subprocess
from time import sleep
from typing import Literal


class ServerHandle:
    """
    A ServerHandle starts a server instance and can be used to stop it.
    """

    EXECUTABLE_PATH = "vanity/cmake-build-debug/vanity"
    STARTUP_DELAY = 0.03

    def __init__(
        self,
        *,
        host: str | None = None,
        port: int | None = None,
        ports: list[int] | None = None,
        cluster_port: int | None = None,
        executable_path: str = EXECUTABLE_PATH,
        env: dict[str, str] = None,
        working_dir: str = None,
        no_db_persist: bool = True,
        no_auth_persist: bool = True,
        no_wal: bool = True,
        no_logging: bool = True,
        log_level: Literal["debug", "info", "warning", "error", "critical"] = None,
    ):
        """
        Create a new ServerHandle.
        :param port: The port to run the server on (any or both of port and ports can be specified)
        :param ports: Extra ports to run the server on (any or both of port and ports can be specified)
        :param executable_path: The path to the server executable.
        :param env: The environment variables to run the server with.
        :param working_dir: The working directory to run the server in (None for no working directory).
        :param no_db_persist: Whether to persist the database.
        :param no_auth_persist: Whether to persist the users file.
        :param no_wal: Whether to use the write-ahead log.
        :param no_logging: Whether to log.
        :param log_level: The level to log at.
        """
        self.args = [executable_path]
        self.env = env

        _ports = set()
        if port is not None:
            _ports.add(port)

        if ports is not None:
            _ports.update(ports)

        self.ports = list(_ports)
        if _ports:
            for port in _ports:
                self.args.append(f"--port={port}")

        if host:
            self.args.append(f"--host={host}")

        if cluster_port:
            self.args.append(f"--cluster-port={cluster_port}")

        if working_dir:
            self.args.append(f"--working-dir={working_dir}")
        else:
            self.args.append("--no-working-dir")

        if no_db_persist:
            self.args.append("--no-db-persist")

        if no_auth_persist:
            self.args.append("--no-auth-persist")

        if no_wal:
            self.args.append("--no-wal")

        if no_logging:
            self.args.append("--no-logging")

        if log_level:
            self.args.append(f"--log-level={log_level}")

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

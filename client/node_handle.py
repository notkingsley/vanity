import socket

from client.client_ import Client
from client.server_handle import ServerHandle


class NodeHandle:
    """
    A handle for a node in a cluster.
    """

    def __init__(
        self,
        host: str | None = None,
        ports: list[int] | None = None,
        cluster_port: int | None = None,
        cluster_id: str | None = None,
        server_args: dict | None = None,
        client_args: dict[str, str | None] | None = None,
    ):
        self.host = host or "localhost"
        self.ports = ports or [self.get_free_port()]
        self.cluster_port = cluster_port or self.get_free_port()
        self.client_args = client_args or self.default_client_args()
        self.server_args = server_args or {}
        self.cluster_id = cluster_id
        self.server = ServerHandle(
            host=self.host,
            ports=self.ports,
            cluster_port=self.cluster_port,
            **self.server_args,
        )

        self.start = self.server.start
        self.stop = self.server.stop
        self.active = self.server.is_running

    @staticmethod
    def get_free_port():
        """
        Get a (likely) free port number.
        """
        s = socket.socket()
        s.bind(("", 0))
        port = s.getsockname()[1]
        s.close()
        return port

    @staticmethod
    def default_client_args():
        """
        Get the default client arguments.
        """
        return {"username": None, "password": None}

    def client(self):
        """
        Create a client for the node.
        """
        return Client(self.host, self.ports[0], **self.client_args)

    def new_cluster(self, cluster_key: str, id: str | None = None):
        """
        Create a new cluster on the node.
        """
        id = id or self.cluster_id
        with self.client() as client:
            response = client.cluster_new(cluster_key, id)
            if not response.is_ok():
                raise RuntimeError(f"Failed to create new cluster: {response.value}")

            self.cluster_id: str = response.value

    def join_cluster(
        self, cluster_key: str, host: str, port: int, id: str | None = None
    ):
        """
        Join a cluster on the node.
        """
        id = id or self.cluster_id
        with self.client() as client:
            response = client.cluster_join(cluster_key, host, port, id)
            if not response.is_ok():
                raise RuntimeError(f"Failed to join cluster: {response.value}")

            self.cluster_id: str = response.value

    def leave_cluster(self):
        """
        Leave a cluster on the node.
        """
        with self.client() as client:
            response = client.cluster_leave()
            if not response.is_ok():
                raise RuntimeError(f"Failed to leave cluster: {response.value}")

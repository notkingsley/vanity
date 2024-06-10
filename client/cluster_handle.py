import secrets
import time

from client.node_handle import NodeHandle


class ClusterHandle:
    """
    A handle for a cluster of nodes.
    """

    def __init__(self, cluster_key: str | None = None, nodes: list[NodeHandle] = []):
        self.cluster_key = cluster_key or secrets.token_hex(16)
        self.nodes = nodes

    def from_configs(
        configs: list[dict],
        cluster_key: str | None = None,
    ) -> "ClusterHandle":
        """
        Create a ClusterHandle from a list of node configurations.
        """
        nodes = [NodeHandle(**config) for config in configs]
        return ClusterHandle(cluster_key, nodes)

    def with_count(
        count: int,
        cluster_key: str | None = None,
    ) -> "ClusterHandle":
        """
        Create a ClusterHandle with a given number of nodes.
        """
        nodes = [NodeHandle() for _ in range(count)]
        return ClusterHandle(cluster_key, nodes)

    @property
    def node_sync_delay(self):
        """
        The delay waiting for nodes to sync.
        """
        return 1

    def wait_for_sync(self):
        """
        Wait for all the nodes in the cluster to sync.
        """
        time.sleep(self.node_sync_delay)

    def start(self):
        """
        Start all the nodes in the cluster.
        """
        for node in self.nodes:
            node.start()

        self.nodes[0].new_cluster(self.cluster_key)
        for node in self.nodes[1:]:
            node.join_cluster(
                self.cluster_key,
                self.nodes[0].host,
                self.nodes[0].cluster_port,
            )

        self.wait_for_sync()

    def stop(self):
        """
        Stop all the nodes in the cluster.
        """
        for node in self.nodes:
            node.leave_cluster()
            node.stop()

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop()
        return False

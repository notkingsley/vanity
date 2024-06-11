import unittest

from client import AuthLevel, ServerHandle
from tests.utils import get_free_port, make_client


class ClusterKeyTest(unittest.TestCase):
    """
    Test the cluster_key, cluster_leave, and cluster_new commands.
    """

    def setUp(self) -> None:
        self.port = get_free_port()
        self.server_handle = ServerHandle(ports=[self.port])
        self.server_handle.start()

    def tearDown(self) -> None:
        self.server_handle.stop()

    def make_client(self):
        """
        Make a client conected to self.port
        """
        return make_client(self.port)

    def test_cluster_new(self):
        """
        Test cluster_new.
        """
        with self.make_client() as client:
            response = client.cluster_new("test_cluster_key")
            self.assertTrue(response.is_ok())
            self.assertEqual(response.value, "test_cluster_key")
            response = client.cluster_key()
            self.assertTrue(response.is_ok())
            self.assertEqual(response.value, "test_cluster_key")

    def test_cluster_new_no_key(self):
        """
        Test cluster_new with no key.
        """
        with self.make_client() as client:
            response = client.cluster_new("")
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "key is too short")

    def test_cluster_new_already_in_cluster(self):
        """
        Test cluster_new when already in a cluster.
        """
        with self.make_client() as client:
            response = client.cluster_new("test_cluster_key")
            self.assertTrue(response.is_ok())
            response = client.cluster_new("test_cluster_key_2")
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "already in a cluster")

    def test_cluster_key_no_cluster(self):
        """
        Test cluster_key when not in a cluster.
        """
        with self.make_client() as client:
            response = client.cluster_key()
            self.assertTrue(response.is_null())

    def test_cluster_key(self):
        """
        Test cluster_key.
        """
        with self.make_client() as client:
            response = client.cluster_new("test_cluster_key")
            self.assertTrue(response.is_ok())
            response = client.cluster_key()
            self.assertTrue(response.is_ok())
            self.assertEqual(response.value, "test_cluster_key")

    def test_cluster_leave(self):
        """
        Test cluster_leave.
        """
        with self.make_client() as client:
            response = client.cluster_new("test_cluster_key")
            self.assertTrue(response.is_ok())
            response = client.cluster_leave()
            self.assertTrue(response.is_ok())
            response = client.cluster_key()
            self.assertTrue(response.is_null())

    def test_cluster_leave_no_cluster(self):
        """
        Test cluster_leave when not in a cluster.
        """
        with self.make_client() as client:
            response = client.cluster_leave()
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "not in a cluster")


class ClusterCommandsNoAdmin(unittest.TestCase):
    """
    Test the cluster commands when not an admin.
    """

    def setUp(self) -> None:
        self.port = get_free_port()
        self.server_handle = ServerHandle(ports=[self.port])
        self.server_handle.start()
        self.admin_client = make_client(self.port)
        self.admin_client.add_user("test_user", "test_user_password")
        self.admin_client.edit_user("test_user", AuthLevel.USER)
        self.client = make_client(self.port, no_login=True)
        self.client.auth("test_user", "test_user_password")

    def tearDown(self) -> None:
        self.client.close()
        self.admin_client.close()
        self.server_handle.stop()

    def test_cluster_new(self):
        """
        Test cluster_new.
        """
        response = self.client.cluster_new("test_cluster_key")
        self.assertTrue(response.is_denied())

    def test_cluster_key(self):
        """
        Test cluster_key.
        """
        response = self.client.cluster_key()
        self.assertTrue(response.is_denied())

    def test_cluster_leave(self):
        """
        Test cluster_leave.
        """
        response = self.client.cluster_leave()
        self.assertTrue(response.is_denied())

    def test_cluster_join(self):
        """
        Test cluster_join.
        """
        response = self.client.cluster_join("test_cluster_key", "badhost", 1234)
        self.assertTrue(response.is_denied())


class ClusterJoinTest(unittest.TestCase):
    """
    Test the cluster_join command.
    """

    PEER_SYNC_DELAY = 0.5

    def setUp(self) -> None:
        self.host = "127.0.0.1"
        self.port1 = get_free_port()
        self.port2 = get_free_port()
        self.port3 = get_free_port()
        self.port4 = get_free_port()
        self.cluster_port1 = get_free_port()
        self.cluster_port2 = get_free_port()
        self.cluster_port3 = get_free_port()
        self.cluster_port4 = get_free_port()
        self.server1 = ServerHandle(
            ports=[self.port1], host=self.host, cluster_port=self.cluster_port1
        )
        self.server2 = ServerHandle(
            ports=[self.port2], host=self.host, cluster_port=self.cluster_port2
        )
        self.server3 = ServerHandle(
            ports=[self.port3], host=self.host, cluster_port=self.cluster_port3
        )
        self.server4 = ServerHandle(
            ports=[self.port4], host=self.host, cluster_port=self.cluster_port4
        )
        self.server1.start()
        self.server2.start()
        self.server3.start()
        self.server4.start()
        self.client1 = make_client(port=self.port1)
        self.client2 = make_client(port=self.port2)
        self.client3 = make_client(port=self.port3)
        self.client4 = make_client(port=self.port4)
        response = self.client1.cluster_new("test_cluster_key")
        self.assertTrue(response.is_ok())

    def tearDown(self) -> None:
        self.client4.close()
        self.client3.close()
        self.client2.close()
        self.client1.close()
        self.server4.stop()
        self.server3.stop()
        self.server2.stop()
        self.server1.stop()

    def wait_for_peers_sync(self):
        import time

        time.sleep(self.PEER_SYNC_DELAY)

    def test_cluster_join(self):
        """
        Test cluster_join, peers, and cluster_leave.
        """
        response = self.client2.cluster_join(
            "test_cluster_key", self.host, self.cluster_port1
        )
        self.assertTrue(response.is_ok())
        response = self.client3.cluster_join(
            "test_cluster_key", self.host, self.cluster_port2
        )
        self.assertTrue(response.is_ok())
        response = self.client4.cluster_join(
            "test_cluster_key", self.host, self.cluster_port3
        )
        self.assertTrue(response.is_ok())

        self.wait_for_peers_sync()

        response = self.client1.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            {
                f"{self.host}:{self.cluster_port2}",
                f"{self.host}:{self.cluster_port3}",
                f"{self.host}:{self.cluster_port4}",
            },
        )

        response = self.client2.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            {
                f"{self.host}:{self.cluster_port1}",
                f"{self.host}:{self.cluster_port3}",
                f"{self.host}:{self.cluster_port4}",
            },
        )

        response = self.client3.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            {
                f"{self.host}:{self.cluster_port1}",
                f"{self.host}:{self.cluster_port2}",
                f"{self.host}:{self.cluster_port4}",
            },
        )

        response = self.client4.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            {
                f"{self.host}:{self.cluster_port1}",
                f"{self.host}:{self.cluster_port2}",
                f"{self.host}:{self.cluster_port3}",
            },
        )

        response = self.client1.cluster_leave()
        self.assertTrue(response.is_ok())
        response = self.client1.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, set())

        self.wait_for_peers_sync()

        response = self.client2.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            {
                f"{self.host}:{self.cluster_port3}",
                f"{self.host}:{self.cluster_port4}",
            },
        )

        response = self.client3.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            {
                f"{self.host}:{self.cluster_port2}",
                f"{self.host}:{self.cluster_port4}",
            },
        )

        response = self.client4.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            {
                f"{self.host}:{self.cluster_port2}",
                f"{self.host}:{self.cluster_port3}",
            },
        )

        response = self.client3.cluster_leave()
        self.assertTrue(response.is_ok())
        response = self.client3.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, set())

        response = self.client2.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, {f"{self.host}:{self.cluster_port4}"})

        response = self.client4.peers()
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, {f"{self.host}:{self.cluster_port2}"})


class ClusterJoinFailTest(unittest.TestCase):
    """
    Test the cluster_join command when it should fail.
    """

    def setUp(self) -> None:
        self.host = "127.0.0.1"
        self.port1 = get_free_port()
        self.port2 = get_free_port()
        self.cluster_port1 = get_free_port()
        self.cluster_port2 = get_free_port()
        self.server1 = ServerHandle(
            ports=[self.port1], host=self.host, cluster_port=self.cluster_port1
        )
        self.server2 = ServerHandle(
            ports=[self.port2], host=self.host, cluster_port=self.cluster_port2
        )
        self.server1.start()
        self.server2.start()
        self.client1 = make_client(port=self.port1)
        self.client2 = make_client(port=self.port2)

    def test_cluster_join_bad_key(self):
        """
        Test cluster_join with a bad key.
        """
        response = self.client1.cluster_new("test_cluster_key")
        self.assertTrue(response.is_ok())
        response = self.client2.cluster_join(
            "bad_cluster_key", self.host, self.cluster_port1
        )
        self.assertTrue(response.is_error())
        self.assertEqual(response.body, "peer denied connection")

    def test_cluster_join_no_cluster(self):
        """
        Test cluster_join when not in a cluster.
        """
        response = self.client2.cluster_join(
            "test_cluster_key", self.host, self.cluster_port1
        )
        self.assertTrue(response.is_error())
        self.assertEqual(response.body, "peer denied connection")

    def test_cluster_join_already_in_cluster(self):
        """
        Test cluster_join when already in a cluster.
        """
        response = self.client1.cluster_new("test_cluster_key")
        self.assertTrue(response.is_ok())
        response = self.client2.cluster_join(
            "test_cluster_key", self.host, self.cluster_port1
        )
        self.assertTrue(response.is_ok())

        port3 = get_free_port()
        cluster_port3 = get_free_port()
        server3 = ServerHandle(
            ports=[port3], host=self.host, cluster_port=cluster_port3
        )

        with server3, make_client(port=port3) as client3:
            response = client3.cluster_new("test_cluster_key_2")
            self.assertTrue(response.is_ok())
            response = client3.cluster_join(
                "test_cluster_key", self.host, self.cluster_port1
            )
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "already in a cluster")

            response = self.client2.cluster_join(
                "test_cluster_key", self.host, self.cluster_port1
            )
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "already in a cluster")


class ClusterJoinNonClusterPort(unittest.TestCase):
    """
    Test that trying to connect a peer to a non cluster port
    will redirect to use the correct (cluster) port
    """

    def setUp(self) -> None:
        self.host = "127.0.0.1"
        self.port1 = get_free_port()
        self.port2 = get_free_port()
        self.cluster_port1 = get_free_port()
        self.cluster_port2 = get_free_port()
        self.server1 = ServerHandle(
            ports=[self.port1], host=self.host, cluster_port=self.cluster_port1
        )
        self.server2 = ServerHandle(
            ports=[self.port2], host=self.host, cluster_port=self.cluster_port2
        )
        self.server1.start()
        self.server2.start()
        self.client1 = make_client(port=self.port1)
        self.client2 = make_client(port=self.port2)

    def test_cluster_join_non_cluster_port(self):
        """
        Test that trying to connect a peer to a non cluster port
        will redirect to use the correct (cluster) port
        """
        response = self.client1.cluster_new("test_cluster_key")
        self.assertTrue(response.is_ok())
        response = self.client2.cluster_join("test_cluster_key", self.host, self.port1)
        self.assertTrue(response.is_ok())

        response = self.client1.peers()
        self.assertEqual(response.value, {f"{self.host}:{self.cluster_port2}"})

        response = self.client2.peers()
        self.assertEqual(response.value, {f"{self.host}:{self.cluster_port1}"})

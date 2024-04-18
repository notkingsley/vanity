import unittest

from client import AuthLevel, ServerHandle
from tests.utils import get_free_port, make_client


class ClusterKeyTest(unittest.TestCase):
    """
    Test the cluster_key, cluster_leave, and cluster_new commands.
    """

    def setUp(self) -> None:
        self.port = get_free_port()
        self.server_handle = ServerHandle(port=self.port)
        self.server_handle.start()

    def tearDown(self) -> None:
        self.server_handle.stop()

    def test_cluster_new(self):
        """
        Test cluster_new.
        """
        with make_client(port=self.port) as client:
            response = client.cluster_new("bad_cluster_key")
            self.assertTrue(response.is_ok())
            self.assertEqual(response.value, "bad_cluster_key")
            response = client.cluster_key()
            self.assertTrue(response.is_ok())
            self.assertEqual(response.value, "bad_cluster_key")

    def test_cluster_new_no_key(self):
        """
        Test cluster_new with no key.
        """
        with make_client(port=self.port) as client:
            response = client.cluster_new("")
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "key is too short")

    def test_cluster_new_already_in_cluster(self):
        """
        Test cluster_new when already in a cluster.
        """
        with make_client(port=self.port) as client:
            response = client.cluster_new("bad_cluster_key")
            self.assertTrue(response.is_ok())
            response = client.cluster_new("bad_cluster_key_2")
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "already in a cluster")

    def test_cluster_key_no_cluster(self):
        """
        Test cluster_key when not in a cluster.
        """
        with make_client(port=self.port) as client:
            response = client.cluster_key()
            self.assertTrue(response.is_null())

    def test_cluster_key(self):
        """
        Test cluster_key.
        """
        with make_client(port=self.port) as client:
            response = client.cluster_new("bad_cluster_key")
            self.assertTrue(response.is_ok())
            response = client.cluster_key()
            self.assertTrue(response.is_ok())
            self.assertEqual(response.value, "bad_cluster_key")

    def test_cluster_leave(self):
        """
        Test cluster_leave.
        """
        with make_client(port=self.port) as client:
            response = client.cluster_new("bad_cluster_key")
            self.assertTrue(response.is_ok())
            response = client.cluster_leave()
            self.assertTrue(response.is_ok())
            response = client.cluster_key()
            self.assertTrue(response.is_null())

    def test_cluster_leave_no_cluster(self):
        """
        Test cluster_leave when not in a cluster.
        """
        with make_client(port=self.port) as client:
            response = client.cluster_leave()
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "not in a cluster")


class ClusterCommandsNoAdmin(unittest.TestCase):
    """
    Test the cluster commands when not an admin.
    """

    def setUp(self) -> None:
        self.port = get_free_port()
        self.server_handle = ServerHandle(port=self.port)
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
        response = self.client.cluster_new("bad_cluster_key")
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

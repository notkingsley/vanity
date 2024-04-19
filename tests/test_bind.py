import unittest

from client import ServerHandle
from tests.utils import get_free_port, make_client


class BindTest(unittest.TestCase):
    """
    Test binding and unbinding a runnning server to a new port.
    """

    HOST = "localhost"

    def setUp(self) -> None:
        self.port = get_free_port()
        self.server_handle = ServerHandle(port=self.port)
        self.server_handle.start()

    def tearDown(self) -> None:
        self.server_handle.stop()

    def test_bind_invalid_port(self):
        """
        Test binding to an invalid port.
        """
        with make_client(port=self.port) as client:
            response = client.bind(self.HOST, 0)
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "invalid port")

        with make_client(port=self.port) as client:
            response = client.bind(self.HOST, 65536)
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "invalid port")

        with make_client(port=self.port) as client:
            response = client.bind(self.HOST, -1)
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "invalid port")

    def test_bind_already_bound(self):
        """
        Test binding to an already bound port.
        """
        with make_client(port=self.port) as client:
            response = client.bind(self.HOST, self.port)
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "already bound to port")

    def test_bind(self):
        """
        Test binding to a new port.
        """
        new_port = get_free_port()
        with make_client(port=self.port) as client:
            response = client.bind(self.HOST, new_port)
            self.assertTrue(response.is_ok())

        with make_client(port=new_port) as client:
            response = client.ping()
            self.assertTrue(response.is_pong())

    def test_unbind_invalid_port(self):
        """
        Test unbinding an invalid port.
        """
        with make_client(port=self.port) as client:
            response = client.unbind(self.HOST, 0)
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "invalid port")

        with make_client(port=self.port) as client:
            response = client.unbind(self.HOST, 65536)
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "invalid port")

        with make_client(port=self.port) as client:
            response = client.unbind(self.HOST, -1)
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "invalid port")

    def test_unbind_not_bound(self):
        """
        Test unbinding a port that is not bound.
        """
        with make_client(port=self.port) as client:
            client.bind(self.HOST, get_free_port())
            response = client.unbind(self.HOST, get_free_port())
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "was not bound to port")

    def test_unbind_last_port(self):
        """
        Test unbinding the last port.
        """
        with make_client(port=self.port) as client:
            response = client.unbind(self.HOST, self.port)
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "cannot unbind last port")

    def test_unbind_cluster_port(self):
        """
        Test unbinding the cluster port.
        """
        server_port = get_free_port()
        cluster_port = get_free_port()
        with ServerHandle(port=server_port, cluster_port=cluster_port):
            with make_client(port=server_port) as client:
                response = client.unbind(self.HOST, cluster_port)
                self.assertTrue(response.is_error())
                self.assertEqual(response.body, "cannot unbind cluster port")

    def test_unbind(self):
        """
        Test unbinding a bound port.
        """
        new_port = get_free_port()
        with make_client(port=self.port) as client:
            response = client.bind(self.HOST, new_port)
            self.assertTrue(response.is_ok())

        with make_client(port=self.port) as client:
            response = client.unbind(self.HOST, new_port)
            self.assertTrue(response.is_ok())

        with self.assertRaises(ConnectionRefusedError):
            with make_client(port=new_port) as client:
                pass

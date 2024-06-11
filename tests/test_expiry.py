import time
from tempfile import TemporaryDirectory
import unittest

from client.server_handle import ServerHandle
from tests.test_db import BaseDatabaseTest
from tests.utils import get_free_port, make_client


class ExpiryTest(BaseDatabaseTest):
    """
    Test the expiry commands
    """

    EXPIRE_TIME = 0.1

    def test_set_expiry(self):
        """
        Test the set_expiry command.
        """
        response = self.client.str_set("key", "value")
        self.assertTrue(response.is_ok())
        response = self.client.set_expiry("key", self.EXPIRE_TIME)
        self.assertTrue(response.is_ok())
        time.sleep(self.EXPIRE_TIME)
        response = self.client.get("key")
        self.assertTrue(response.is_null())

    def test_get_expiry(self):
        """
        Test the get_expiry command.
        """
        response = self.client.str_set("key", "value")
        self.assertTrue(response.is_ok())
        response = self.client.set_expiry("key", self.EXPIRE_TIME)
        self.assertTrue(response.is_ok())
        response = self.client.get_expiry("key")
        self.assertTrue(response.is_ok())
        self.assertAlmostEqual(response.value, self.EXPIRE_TIME, 2)

    def test_clear_expiry(self):
        """
        Test the clear_expiry command.
        """
        response = self.client.str_set("key", "value")
        self.assertTrue(response.is_ok())
        response = self.client.set_expiry("key", self.EXPIRE_TIME)
        self.assertTrue(response.is_ok())
        response = self.client.clear_expiry("key")
        self.assertTrue(response.is_ok())
        time.sleep(self.EXPIRE_TIME)
        response = self.client.get("key")
        self.assertEqual(response.value, "value")


class WalExpiryTest(unittest.TestCase):
    """
    Test the expiry commands with the WAL enabled.
    """

    EXPIRE_TIME = 0.1

    def setUp(self) -> None:
        self.temp_dir = TemporaryDirectory()
        self.port = get_free_port()
        self.server_handle = ServerHandle(
            ports=[self.port], no_wal=False, working_dir=self.temp_dir.name
        )
        self.server_handle.start()

    def tearDown(self):
        self.server_handle.stop()
        self.temp_dir.cleanup()

    def test_expired_key_across_reboot(self):
        """
        Test that a key expired prior to restart is absent
        after WAL recovery
        """
        with make_client(self.port) as client:
            response = client.str_set("key", "test_expired_key_across_reboot")
            self.assertTrue(response.is_ok())
            response = client.set_expiry("key", self.EXPIRE_TIME)
            self.assertTrue(response.is_ok())
            time.sleep(self.EXPIRE_TIME)
            response = client.get("key")
            self.assertTrue(response.is_null())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("key")
            self.assertTrue(response.is_null())

    def test_expiry_correctness_across_reboot(self):
        """
        Test that WAL does not reset expiry times across reboots
        """
        with make_client(self.port) as client:
            response = client.str_set("key", "test_expiry_correctness_across_reboot")
            self.assertTrue(response.is_ok())
            response = client.set_expiry("key", self.EXPIRE_TIME)
            self.assertTrue(response.is_ok())

        self.server_handle.stop()
        time.sleep(self.EXPIRE_TIME)
        self.server_handle.start()

        with make_client(self.port) as client:
            response = client.get("key")
            self.assertTrue(response.is_null())

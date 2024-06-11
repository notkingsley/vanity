import unittest

from tests.utils import get_free_port, make_client
from client import Pipe, ServerHandle


class TestPipe(unittest.TestCase):
    """
    Test the Pipe class.
    """

    @classmethod
    def setUpClass(cls) -> None:
        port = get_free_port()
        cls.server_handle = ServerHandle(ports=[port])
        cls.server_handle.start()
        cls.client = make_client(port)

    @classmethod
    def tearDownClass(cls) -> None:
        cls.client.close()
        cls.server_handle.stop()

    def setUp(self) -> None:
        self.pipe = Pipe(self.client)

    def test_pipe(self):
        """
        Test the pipe.
        """
        self.pipe.get("test")
        self.pipe.str_set("test", "testval")
        self.pipe.get("test")
        self.pipe.delete("test")
        self.pipe.get("test")

        response = self.pipe.execute()

        response1 = response.value[0]
        self.assertTrue(response1.is_null())

        response2 = response.value[1]
        self.assertTrue(response2.is_ok())

        response3 = response.value[2]
        self.assertTrue(response3.is_ok())
        self.assertEqual(response3.value, "testval")

        response4 = response.value[3]
        self.assertTrue(response4.is_ok())

        response5 = response.value[4]
        self.assertTrue(response5.is_null())

    def test_pipe_switch_db(self):
        """
        Test the pipe while switching the database.
        """
        self.pipe.switch_db(1)
        self.pipe.str_set("test", "testval")
        self.pipe.switch_db(0)
        self.pipe.get("test")
        self.pipe.switch_db(1)
        self.pipe.get("test")

        response = self.pipe.execute()

        response1 = response.value[0]
        self.assertTrue(response1.is_ok())

        response2 = response.value[1]
        self.assertTrue(response2.is_ok())

        response3 = response.value[2]
        self.assertTrue(response3.is_ok())

        response4 = response.value[3]
        self.assertTrue(response4.is_null())

        response5 = response.value[4]
        self.assertTrue(response5.is_ok())

        response6 = response.value[5]
        self.assertTrue(response6.is_ok())
        self.assertEqual(response6.value, "testval")

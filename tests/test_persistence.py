import unittest
from tempfile import TemporaryDirectory

from client.server_handle import ServerHandle
from tests.utils import get_free_port, make_client


class NoPersistenceTest(unittest.TestCase):
    """
    Test no persistence across reboots
    """

    def setUp(self) -> None:
        self.port = get_free_port()
        self.server_handle = ServerHandle(ports=[self.port], no_db_persist=True)
        self.server_handle.start()

    def tearDown(self) -> None:
        self.server_handle.stop()

    def test_no_db_persist(self):
        """
        Test that we can set a value on no_db_persist, restart the server, and get a null.
        """
        with make_client(self.port) as client:
            client.str_set("test_no_db_persist", "test_no_db_persist_value")
            response = client.persist()
            self.assertTrue(response.is_error())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("test_no_db_persist")
            self.assertTrue(response.is_null())


class PersistenceTest(unittest.TestCase):
    """
    Test persistence across reboots
    """

    def setUp(self) -> None:
        self.tmp_dir = TemporaryDirectory()
        self.port = get_free_port()
        self.server_handle = ServerHandle(
            ports=[self.port],
            no_db_persist=False,
            working_dir=self.tmp_dir.name,
        )
        self.server_handle.start()

    def tearDown(self) -> None:
        self.server_handle.stop()
        self.tmp_dir.cleanup()

    def test_persist(self):
        """
        Test that we can set a value on persist, restart the server, and get the value.
        """
        with make_client(self.port) as client:
            client.str_set("test_persist", "test_persist_value")
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("test_persist")
            self.assertEqual(response.value, "test_persist_value")

    def test_persist_int(self):
        """
        Test that we can set an integer value on persist,
        restart the server, and get the value.
        """
        with make_client(self.port) as client:
            client.int_set("test_persist_int", 123)
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("test_persist_int")
            self.assertEqual(response.value, 123)

    def test_persist_float(self):
        """
        Test that we can set a float value on persist,
        restart the server, and get the value.
        """
        with make_client(self.port) as client:
            client.float_set("test_persist_float", 123.456)
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("test_persist_float")
            self.assertEqual(response.value, 123.456)

    def test_persist_list(self):
        """
        Test that we can set a list value on persist,
        restart the server, and get the value.
        """
        with make_client(self.port) as client:
            client.list_push_right("test_persist_list", ["red", "green", "blue"])
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.list_range("test_persist_list", 0, -1)
            self.assertEqual(response.value, ["red", "green", "blue"])

    def test_persist_set(self):
        """
        Test that we can set a set value on persist,
        restart the server, and get the value.
        """
        with make_client(self.port) as client:
            client.set_add("test_persist_set", {"red", "green", "blue"})
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.set_all("test_persist_set")
            self.assertEqual(response.value, {"red", "green", "blue"})

    def test_persist_hash(self):
        """
        Test that we can set a hash value on persist,
        restart the server, and get the value.
        """
        with make_client(self.port) as client:
            response = client.hash_set(
                "test_persist_hash", {"red": "1", "green": "2", "blue": "3"}
            )
            self.assertTrue(response.is_ok())
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.hash_all("test_persist_hash")
            self.assertEqual(response.value, {"red": "1", "green": "2", "blue": "3"})


class WALPersistenceDisabledTest(unittest.TestCase):
    """
    Test that WAL recovery works if persistence is disabled.
    """

    def setUp(self) -> None:
        self.tmp_dir = TemporaryDirectory()
        self.port = get_free_port()
        self.server_handle = ServerHandle(
            ports=[self.port],
            no_db_persist=True,
            no_wal=False,
            working_dir=self.tmp_dir.name,
        )
        self.server_handle.start()

    def tearDown(self) -> None:
        self.server_handle.stop()
        self.tmp_dir.cleanup()

    def test_wal_persist(self):
        """
        Test that we can set a value on wal persist, restart the server, and get the value.
        """
        with make_client(self.port) as client:
            response = client.str_set("test_wal_persist", "test_wal_persist_value")
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("test_wal_persist")
            self.assertEqual(response.value, "test_wal_persist_value")

    def test_wal_persist_int(self):
        """
        Test that we can set an integer value on wal persist,
        restart the server, and get the value.
        """
        with make_client(self.port) as client:
            response = client.int_set("test_wal_persist_int", 123)
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("test_wal_persist_int")
            self.assertEqual(response.value, 123)

    def test_wal_persist_float(self):
        """
        Test that we can set a float value on wal persist,
        restart the server, and get the value.
        """
        with make_client(self.port) as client:
            response = client.float_set("test_wal_persist_float", 123.456)
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("test_wal_persist_float")
            self.assertEqual(response.value, 123.456)

    def test_wal_persist_list(self):
        """
        Test that we can set a list value on wal persist,
        restart the server, and get the value.
        """
        with make_client(self.port) as client:
            response = client.list_push_right(
                "test_wal_persist_list", ["red", "green", "blue"]
            )
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.list_range("test_wal_persist_list", 0, -1)
            self.assertEqual(response.value, ["red", "green", "blue"])

    def test_wal_persist_set(self):
        """
        Test that we can set a set value on wal persist,
        restart the server, and get the value.
        """
        with make_client(self.port) as client:
            response = client.set_add("test_wal_persist_set", {"red", "green", "blue"})
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.set_all("test_wal_persist_set")
            self.assertEqual(response.value, {"red", "green", "blue"})

    def test_wal_persist_hash(self):
        """
        Test that we can set a hash value on wal persist,
        restart the server, and get the value.
        """
        with make_client(self.port) as client:
            response = client.hash_set(
                "test_wal_persist_hash", {"red": "1", "green": "2", "blue": "3"}
            )
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.hash_all("test_wal_persist_hash")
            self.assertEqual(response.value, {"red": "1", "green": "2", "blue": "3"})


class WALPersistenceTest(unittest.TestCase):
    """
    Test that WAL recovery works if persistence is enabled.
    """

    def setUp(self) -> None:
        self.temp_dir = TemporaryDirectory()
        self.port = get_free_port()
        self.server_handle = ServerHandle(
            ports=[self.port],
            no_db_persist=False,
            no_wal=False,
            working_dir=self.temp_dir.name,
        )
        self.server_handle.start()

    def tearDown(self) -> None:
        self.server_handle.stop()
        self.temp_dir.cleanup()

    def test_wal_persist(self):
        """
        Test that we can set a value on wal persist, restart the server, and get the value.
        """
        with make_client(self.port) as client:
            response = client.str_set("test_wal_persist", "test_wal_persist_value")
            self.assertTrue(response.is_ok())
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("test_wal_persist")
            self.assertEqual(response.value, "test_wal_persist_value")

    def test_wal_persist_int_incr(self):
        """
        Test that we can set an integer value, increment it,
        restart the server, and get the correct value.
        """
        with make_client(self.port) as client:
            response = client.int_set("test_wal_persist_int_incr", 123)
            self.assertTrue(response.is_ok())
            response = client.incr_int("test_wal_persist_int_incr")
            self.assertEqual(response.value, 124)
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("test_wal_persist_int_incr")
            self.assertEqual(response.value, 124)

    def test_wal_persist_float_incr(self):
        """
        Test that we can set a float value, increment it,
        restart the server, and get the correct value.
        """
        with make_client(self.port) as client:
            response = client.float_set("test_wal_persist_float_incr", 123.456)
            self.assertTrue(response.is_ok())
            response = client.incr_float("test_wal_persist_float_incr")
            self.assertEqual(response.value, 124.456)
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.get("test_wal_persist_float_incr")
            self.assertEqual(response.value, 124.456)

    def test_wal_persist_list_push(self):
        """
        Test that we can set a list value, push to it,
        restart the server, and get the correct value.
        """
        with make_client(self.port) as client:
            response = client.list_push_right(
                "test_wal_persist_list_push", ["red", "green", "blue"]
            )
            self.assertTrue(response.is_ok())
            response = client.list_push_right(
                "test_wal_persist_list_push", ["yellow", "purple"]
            )
            self.assertTrue(response.is_ok())
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.list_range("test_wal_persist_list_push", 0, -1)
            self.assertEqual(
                response.value, ["red", "green", "blue", "yellow", "purple"]
            )

    def test_wal_persist_set_add(self):
        """
        Test that we can set a set value, add to it,
        restart the server, and get the correct value.
        """
        with make_client(self.port) as client:
            response = client.set_add(
                "test_wal_persist_set_add", {"red", "green", "blue"}
            )
            self.assertTrue(response.is_ok())
            response = client.set_add("test_wal_persist_set_add", {"yellow", "purple"})
            self.assertTrue(response.is_ok())
            response = client.persist()
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port) as client:
            response = client.set_all("test_wal_persist_set_add")
            self.assertEqual(
                response.value, {"red", "green", "blue", "yellow", "purple"}
            )

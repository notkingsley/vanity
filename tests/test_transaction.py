import unittest

from client import AuthLevel, Pipe, ServerHandle, Transaction
from tests.utils import get_free_port, make_client


class TestTransaction(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.port = get_free_port()
        cls.server_handle = ServerHandle(port=cls.port)
        cls.server_handle.start()

    @classmethod
    def tearDownClass(cls) -> None:
        cls.server_handle.stop()

    def setUp(self) -> None:
        self.client = make_client(self.port)

    def tearDown(self):
        self.client.reset()
        self.client.close()

    def test_transaction(self):
        """
        Test the transaction.
        """
        with Transaction(self.client) as transaction:
            self.assertTrue(transaction.str_set("test", "testval").is_queued())
            self.assertTrue(transaction.get("test").is_queued())
            self.assertTrue(transaction.str_set("foo", "bar").is_queued())
            self.assertTrue(transaction.delete("test").is_queued())
            self.assertTrue(transaction.get("test").is_queued())

        self.assertTrue(transaction.result.is_ok())
        self.assertTrue(transaction.result.value[0].is_ok())
        self.assertTrue(transaction.result.value[1].is_ok())
        self.assertEqual(transaction.result.value[1].value, "testval")
        self.assertTrue(transaction.result.value[2].is_ok())
        self.assertTrue(transaction.result.value[3].is_ok())
        self.assertTrue(transaction.result.value[4].is_null())

        response = self.client.get("foo")
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "bar")

    def test_transaction_discard(self):
        """
        Test discarding the transaction.
        """
        with Transaction(self.client) as transaction:
            self.assertTrue(transaction.str_set("test", "testval").is_queued())
            self.assertTrue(transaction.get("test").is_queued())
            self.assertTrue(transaction.str_set("foo", "bar").is_queued())
            self.assertTrue(transaction.delete("test").is_queued())
            self.assertTrue(transaction.get("test").is_queued())
            transaction.discard()

        self.assertEqual(transaction.result, None)
        response = self.client.get("foo")
        self.assertTrue(response.is_null())

    def test_transaction_pipe(self):
        """
        Test that we can't use a pipe inside a transaction.
        """
        with Transaction(self.client) as transaction:
            transaction.str_set("test", "testval")
            pipe = Pipe(self.client)
            pipe.str_set("test", "testval")
            pipe.get("test")
            response = pipe.execute()
            self.assertTrue(response.is_bad_state())
            transaction.str_set("foo", "bar")

        self.assertTrue(transaction.result.is_ok())
        self.assertTrue(transaction.result.value[0].is_ok())
        self.assertTrue(transaction.result.value[1].is_ok())

        self.assertEqual(self.client.get("test").value, "testval")
        self.assertEqual(self.client.get("foo").value, "bar")

    def test_transaction_nested(self):
        """
        Test that we can't nest transactions.
        """
        self.client.transact_begin()
        self.client.str_set("test", "testval")
        response = self.client.transact_begin()
        self.assertTrue(response.is_bad_state())
        self.client.str_set("foo", "bar")
        response = self.client.transact_commit()
        self.assertTrue(response.is_ok())
        self.assertTrue(response.value[0].is_ok())
        self.assertTrue(response.value[1].is_ok())

        self.assertEqual(self.client.get("test").value, "testval")
        self.assertEqual(self.client.get("foo").value, "bar")

        response = self.client.transact_commit()
        self.assertTrue(response.is_bad_state())

    def test_transaction_auth(self):
        """
        Test that we can't use auth commands in a transaction.
        """
        with Transaction(self.client) as transaction:
            response = transaction.auth("test", "test")
            self.assertTrue(response.is_bad_state())
            response = transaction.add_user("test", "test")
            self.assertTrue(response.is_bad_state())
            response = transaction.edit_user("test", AuthLevel.ADMIN)
            self.assertTrue(response.is_bad_state())
            response = transaction.del_user("test")
            self.assertTrue(response.is_bad_state())
            response = transaction.change_password("test")
            self.assertTrue(response.is_bad_state())

        self.assertEqual(transaction.result.value, [])

    def test_transaction_switch_db(self):
        """
        Test that we can't switch databases in a transaction.
        """
        with Transaction(self.client) as transaction:
            response = transaction.switch_db(1)
            self.assertTrue(response.is_bad_state())

        self.assertEqual(transaction.result.value, [])

    def test_transaction_multi_db_commands(self):
        """
        Test that we can't use multi-db commands in a transaction.
        """
        self.client.str_set("test", "testval")
        with Transaction(self.client) as transaction:
            response = transaction.copy_to_db("test", 1)
            self.assertTrue(response.is_bad_state())
            response = transaction.move_to_db("test", 1)
            self.assertTrue(response.is_bad_state())

        self.assertEqual(transaction.result.value, [])

    def test_transaction_ping(self):
        """
        Test that we can use ping in a transaction.
        """
        with Transaction(self.client) as transaction:
            response = transaction.ping()
            self.assertTrue(response.is_pong())

        self.assertEqual(transaction.result.value, [])

    def test_transaction_in_pipe(self):
        """
        Test that we can use a transaction in a pipe.
        """
        pipe = Pipe(self.client)
        pipe.str_set("test", "testval")
        with Transaction(pipe) as transaction:
            response = transaction.str_set("foo", "bar")
            self.assertTrue(response is None)
            response = transaction.get("test")
            self.assertTrue(response is None)

        self.assertTrue(transaction.result is None)
        pipe.get("foo")
        response = pipe.execute()
        self.assertTrue(response.is_ok())
        self.assertTrue(response.type_is_agg())
        self.assertTrue(response.value[0].is_ok())  # str_set test
        self.assertTrue(response.value[1].is_ok())  # transact_begin
        self.assertTrue(response.value[2].is_queued())  # str_set foo
        self.assertTrue(response.value[3].is_queued())  # get test
        self.assertTrue(response.value[4].is_ok())  # transact_commit
        self.assertTrue(response.value[4].type_is_agg()) # transact_commit
        self.assertTrue(response.value[4].value[0].is_ok()) # str_set foo
        self.assertTrue(response.value[4].value[1].is_ok()) # get test
        self.assertEqual(response.value[4].value[1].value, "testval")  # get test
        self.assertEqual(response.value[5].value, "bar")  # get foo

        self.assertEqual(self.client.get("test").value, "testval")
        self.assertEqual(self.client.get("foo").value, "bar")

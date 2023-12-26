from client.response import Response
from client.command_interface import CommandInterface


class TransactionFinishedError(Exception):
    """
    An exception thrown when a transaction is finished.
    """

    pass


class Transaction(CommandInterface):
    """
    A Transaction makes executing transactions easier, by automatically
    starting and committing the transaction, or discarding it if an error occurs.
    """

    def __init__(self, client: CommandInterface):
        self.client = client
        self.result = None
        self._finished = False

    def __enter__(self):
        self.client.transact_begin()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self._finished:
            return

        if exc_type is None:
            self.commit()
        else:
            self.discard()

    def request(self, command: str, *args) -> Response | None:
        """
        Send a request to the server.
        :param command: The command to send.
        :param args: The arguments to send.
        :return: The response from the server.
        """
        return self.client.request(command, *args)

    def commit(self):
        """
        Commit the transaction.
        """
        if self._finished:
            raise TransactionFinishedError()

        self.result = self.client.transact_commit()
        self._finished = True

    def discard(self):
        """
        Discard the transaction.
        """
        if self._finished:
            raise TransactionFinishedError()

        self.client.transact_discard()
        self._finished = True

    def transact_begin(self):
        raise RuntimeError("Transactions cannot be nested")

    def transact_commit(self):
        raise RuntimeError(
            "transact_commit() should not be called directly "
            "on a Transaction, use commit() instead"
        )

    def transact_discard(self):
        raise RuntimeError(
            "transact_discard() should not be called directly "
            "on a Transaction, use discard() instead"
        )

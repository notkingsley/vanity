from client.request_interface import RequestInterface, RT


class TransactionFinishedError(Exception):
    """
    An exception thrown when a transaction is finished.
    """

    pass


class Transaction(RequestInterface[RT]):
    """
    A Transaction makes executing transactions easier, by automatically
    starting and committing the transaction, or discarding it if an error occurs.
    """

    def __init__(self, client: RequestInterface[RT]):
        self.client = client
        self._finished = False
        self.result: RT | None = None

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

    def request(self, command: str, *args) -> RT:
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

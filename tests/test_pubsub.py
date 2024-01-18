import unittest

from client import ServerHandle
from client.async_response import PublishData
from tests.utils import get_free_port, make_client


class TestPubSub(unittest.TestCase):
    """
    Test the pubsub functionality.
    """

    def setUp(self) -> None:
        self.port = get_free_port()
        self.server_handle = ServerHandle(port=self.port)
        self.server_handle.start()
        self.client = make_client(self.port)

    def tearDown(self) -> None:
        self.client.close()
        self.server_handle.stop()

    def _publish(self, channel: str, message: str):
        """
        Publish a message.
        :param channel: The channel to publish to.
        :param message: The message to publish.
        """
        with make_client(self.port) as client:
            response = client.publish(channel, message)
            self.assertTrue(response.is_ok())

    def _load_async(self):
        """
        Load all pending asynchronous responses by making a synchronous request.
        """
        response = self.client.ping()
        self.assertTrue(response.is_pong())

    def test_pubsub(self):
        """
        Test the pubsub.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())
        self._publish("test", "testval")
        response = self.client.read_async()
        self.assertTrue(response.is_async())
        self.assertTrue(isinstance(response.data, PublishData))
        self.assertEqual(response.data.channel, "test")
        self.assertEqual(response.data.message, "testval")

    def test_pubsub_self(self):
        """
        Test the pubsub with the same client.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        response = self.client.publish("test", "testval")
        self.assertTrue(response.is_ok())

        response = self.client.read_async()
        self.assertTrue(response.is_async())
        self.assertTrue(isinstance(response.data, PublishData))
        self.assertEqual(response.data.channel, "test")
        self.assertEqual(response.data.message, "testval")

    def test_pubsub_multiple_subscribers(self):
        """
        Test the pubsub with multiple subscribers.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        with make_client(self.port) as client:
            response = client.subscribe("test")
            self.assertTrue(response.is_ok())

            self._publish("test", "testval")

            response = client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "test")
            self.assertEqual(response.data.message, "testval")

            response = self.client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "test")
            self.assertEqual(response.data.message, "testval")

    def test_pubsub_multiple_channels(self):
        """
        Test the pubsub with multiple channels.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        response = self.client.subscribe("foo")
        self.assertTrue(response.is_ok())

        self._publish("test", "testval")
        self._publish("foo", "bar")
        self._load_async()

        response = self.client.read_async()
        self.assertTrue(response.is_async())
        self.assertTrue(isinstance(response.data, PublishData))
        self.assertEqual(response.data.channel, "test")
        self.assertEqual(response.data.message, "testval")

        response = self.client.read_async()
        self.assertTrue(response.is_async())
        self.assertTrue(isinstance(response.data, PublishData))
        self.assertEqual(response.data.channel, "foo")
        self.assertEqual(response.data.message, "bar")

    def test_pubsub_multiple_messages(self):
        """
        Test the pubsub with multiple messages.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        self._publish("test", "testval")
        self._publish("test", "testval2")

        response = self.client.read_async()
        self.assertTrue(response.is_async())
        self.assertTrue(isinstance(response.data, PublishData))
        self.assertEqual(response.data.channel, "test")
        self.assertEqual(response.data.message, "testval")

        response = self.client.read_async()
        self.assertTrue(response.is_async())
        self.assertTrue(isinstance(response.data, PublishData))
        self.assertEqual(response.data.channel, "test")
        self.assertEqual(response.data.message, "testval2")

    def test_pubsub_multiple_subscribers_multiple_messages(self):
        """
        Test the pubsub with multiple subscribers and multiple messages.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        with make_client(self.port) as client:
            response = client.subscribe("test")
            self.assertTrue(response.is_ok())

            self._publish("test", "testval")
            self._publish("test", "testval2")

            response = client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "test")
            self.assertEqual(response.data.message, "testval")

            response = self.client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "test")
            self.assertEqual(response.data.message, "testval")

            response = client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "test")
            self.assertEqual(response.data.message, "testval2")

            response = self.client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "test")
            self.assertEqual(response.data.message, "testval2")

    def test_pubsub_multiple_subscribers_multiple_messages_multiple_channels(self):
        """
        Test the pubsub with multiple subscribers and multiple messages and multiple channels.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        response = self.client.subscribe("foo")
        self.assertTrue(response.is_ok())

        with make_client(self.port) as client:
            response = client.subscribe("test")
            self.assertTrue(response.is_ok())

            response = client.subscribe("foo")
            self.assertTrue(response.is_ok())

            self._publish("test", "testval")
            self._publish("test", "testval2")
            self._publish("foo", "bar")
            self._publish("foo", "bar2")

            response = client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "test")
            self.assertEqual(response.data.message, "testval")

            response = self.client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "test")
            self.assertEqual(response.data.message, "testval")

            response = client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "test")
            self.assertEqual(response.data.message, "testval2")

            response = self.client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "test")
            self.assertEqual(response.data.message, "testval2")

            response = client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "foo")
            self.assertEqual(response.data.message, "bar")

            response = self.client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "foo")
            self.assertEqual(response.data.message, "bar")

            response = client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "foo")
            self.assertEqual(response.data.message, "bar2")

            response = self.client.read_async()
            self.assertTrue(response.is_async())
            self.assertTrue(isinstance(response.data, PublishData))
            self.assertEqual(response.data.channel, "foo")
            self.assertEqual(response.data.message, "bar2")

    def test_unsubscribe(self):
        """
        Test unsubscribing.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        response = self.client.unsubscribe("test")
        self.assertTrue(response.is_ok())

        self._publish("test", "testval")

        self.client.subscribe("test2")
        self._publish("test2", "testval2")
        response = self.client.read_async()
        self.assertTrue(response.is_async())
        self.assertTrue(isinstance(response.data, PublishData))
        self.assertEqual(response.data.channel, "test2")
        self.assertEqual(response.data.message, "testval2")

    def test_subscribe_twice(self):
        """
        Test subscribing twice.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        self._publish("test", "testval")

        response = self.client.read_async()
        self.assertTrue(response.is_async())
        self.assertTrue(isinstance(response.data, PublishData))
        self.assertEqual(response.data.channel, "test")
        self.assertEqual(response.data.message, "testval")

        self.client.unsubscribe("test")
        self._publish("test", "testval2")

        self.client.subscribe("test2")
        self._publish("test2", "testval3")
        response = self.client.read_async()
        self.assertTrue(response.is_async())
        self.assertTrue(isinstance(response.data, PublishData))
        self.assertEqual(response.data.channel, "test2")
        self.assertEqual(response.data.message, "testval3")

    def test_sync_response_in_pubsub(self):
        """
        Test that pubsub works with sync responses.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        self._publish("test", "testval")

        response = self.client.str_set("test", "testval")
        self.assertTrue(response.is_ok())

        response = self.client.read_async(block=False)
        self.assertTrue(response.is_async())
        self.assertTrue(isinstance(response.data, PublishData))
        self.assertEqual(response.data.channel, "test")
        self.assertEqual(response.data.message, "testval")

    def test_get_subscribed_channels(self):
        """
        Test getting subscribed channels.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        response = self.client.subscribe("foo")
        self.assertTrue(response.is_ok())

        response = self.client.subscribed()
        self.assertTrue(response.is_ok())
        self.assertTrue(response.type_is_array())
        self.assertEqual(set(response.value), {"test", "foo"})

        response = self.client.unsubscribe("test")
        self.assertTrue(response.is_ok())

        response = self.client.subscribed()
        self.assertTrue(response.is_ok())
        self.assertTrue(response.type_is_array())
        self.assertEqual(response.value, ["foo"])

    def test_unsubscribe_all(self):
        """
        Test unsubscribing all channels.
        """
        response = self.client.subscribe("test")
        self.assertTrue(response.is_ok())

        response = self.client.subscribe("foo")
        self.assertTrue(response.is_ok())

        response = self.client.unsubscribe_all()
        self.assertTrue(response.is_ok())

        self._publish("test", "testval")
        self._publish("foo", "bar")
        self._load_async()

        response = self.client.read_async(block=False)
        self.assertIsNone(response)

        response = self.client.subscribed()
        self.assertTrue(response.is_ok())
        self.assertTrue(response.type_is_array())
        self.assertEqual(response.value, [])

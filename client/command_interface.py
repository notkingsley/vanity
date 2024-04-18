from abc import ABC, abstractmethod

from client.auth_level import AuthLevel
from client.response import Response


class CommandInterface(ABC):
    """
    A CommandInterface defines the interface for handling commands.
    """

    @abstractmethod
    def request(self, command: str, *args) -> Response | None:
        """
        Handle a request.
        :param command: The command to handle.
        :param args: The arguments to handle.
        :return: The response to the request.
        """
        raise NotImplementedError()

    def get(self, key: str):
        """
        Get the value of a key.
        :param key: The key to get.
        :return: The value of the key.
        """
        return self.request("GET", key)

    def str_set(self, key: str, value: str):
        """
        Set the value of a key to a string.
        :param key: The key to set.
        :param value: The value to set.
        """
        return self.request("STR_SET", key, value)

    def int_set(self, key: str, value: int):
        """
        Set the value of a key to an integer.
        :param key: The key to set.
        :param value: The value to set.
        """
        return self.request("INT_SET", key, value)

    def float_set(self, key: str, value: float):
        """
        Set the value of a key to a float.
        :param key: The key to set.
        :param value: The value to set.
        """
        return self.request("FLOAT_SET", key, value)

    def delete(self, key: str):
        """
        Delete a key.
        :param key: The key to delete.
        """
        return self.request("DEL", key)

    def ping(self):
        """
        Ping the server.
        """
        return self.request("PING")

    def persist(self):
        """
        Persist the database.
        """
        return self.request("PERSIST")

    def reset(self):
        """
        Reset the database.
        """
        return self.request("RESET")

    def auth(self, username: str, password: str):
        """
        Authenticate with the server.
        :param username: The username to authenticate with.
        :param password: The password to authenticate with.
        """
        return self.request("AUTH", username, password)

    def switch_db(self, db: int):
        """
        Switch to a different database index
        :param db: The database to switch to.
        """
        return self.request("SWITCH_DB", db)

    def add_user(self, username: str, password: str):
        """
        Add a user to the server.
        :param username: The username to add.
        :param password: The password to add.
        """
        return self.request("ADD_USER", username, password)

    def edit_user(self, username: str, auth_level: AuthLevel):
        """
        Edit a user's auth level on the server.
        :param username: The username to edit.
        :param auth_level: The new auth level.
        """
        return self.request("EDIT_USER", username, auth_level)

    def del_user(self, username: str):
        """
        Delete a user from the server.
        :param username: The username to delete.
        """
        return self.request("DEL_USER", username)

    def change_password(self, password: str):
        """
        Change the current client's password.
        :param password: The new password.
        """
        return self.request("CHANGE_PASSWORD", password)

    def auth_level(self):
        """
        Get the current client's auth level.
        """
        return self.request("AUTH_LEVEL")

    def username(self):
        """
        Get the current client's username.
        """
        return self.request("USERNAME")

    def type(self, key: str):
        """
        Get the type of a key.
        :param key: The key to get the type of.
        :return: The type of the key.
        """
        return self.request("TYPE", key)

    def exists(self, key: str):
        """
        Check if a key exists.
        :param key: The key to check.
        :return: Whether the key exists.
        """
        return self.request("EXISTS", key)

    def keys(self):
        """
        Get all the keys in the database.
        :return: The keys in the database.
        """
        return self.request("KEYS")

    def copy_to(self, source: str, dest: str):
        """
        Copy a key to another key.
        :param source: The key to copy.
        :param dest: The key to copy to.
        """
        return self.request("COPY_TO", source, dest)

    def move_to(self, source: str, dest: str):
        """
        Move a key to another key.
        :param source: The key to move.
        :param dest: The key to move to.
        """
        return self.request("MOVE_TO", source, dest)

    def copy_to_db(self, source: str, db: int):
        """
        Copy a key to another database.
        :param source: The key to copy.
        :param db: The database to copy to.
        """
        return self.request("COPY_TO_DB", source, db)

    def move_to_db(self, source: str, db: int):
        """
        Move a key to another database.
        :param source: The key to move.
        :param db: The database to move to.
        """
        return self.request("MOVE_TO_DB", source, db)

    def set_expiry(self, key: str, expiry: float):
        """
        Set the expiry of a key.
        :param key: The key to set the expiry of.
        :param expiry: The expiry to set, in seconds.
        """
        return self.request("SET_EXPIRY", key, expiry)

    def get_expiry(self, key: str):
        """
        Get the expiry of a key.
        :param key: The key to get the expiry of.
        :return: The expiry of the key, in seconds.
        """
        return self.request("GET_EXPIRY", key)

    def clear_expiry(self, key: str):
        """
        Clear the expiry of a key.
        :param key: The key to clear the expiry of.
        """
        return self.request("CLEAR_EXPIRY", key)

    def incr_int(self, key: str, by: int = 1):
        """
        Increment an integer value.
        :param key: The key to increment.
        :return: The incremented value.
        """
        return self.request("INCR_INT", key, by)

    def decr_int(self, key: str, by: int = 1):
        """
        Decrement an integer value.
        :param key: The key to decrement.
        :return: The decremented value.
        """
        return self.incr_int(key, -by)

    def incr_float(self, key: str, by: float = 1.0):
        """
        Increment a float value.
        :param key: The key to increment.
        :return: The incremented value.
        """
        return self.request("INCR_FLOAT", key, by)

    def decr_float(self, key: str, by: float = 1.0):
        """
        Decrement a float value.
        :param key: The key to decrement.
        :return: The decremented value.
        """
        return self.incr_float(key, -by)

    def str_len(self, key: str):
        """
        Get the length of a string.
        :param key: The key to get the length of.
        :return: The length of the string.
        """
        return self.request("STR_LEN", key)

    def many_get(self, *keys):
        """
        Get the values of many keys.
        :param keys: The keys to get.
        :return: The values of the keys.
        """
        return self.request("MANY_GET", list(keys))

    def list_len(self, key: str):
        """
        Get the length of a list.
        :param key: The key to get the length of.
        :return: The length of the list.
        """
        return self.request("LIST_LEN", key)

    def list_get(self, key: str, index: int):
        """
        Get an item from a list.
        :param key: The key of the list.
        :param index: The index of the item.
        :return: The item at the index.
        """
        return self.request("LIST_GET", key, index)

    def list_set(self, key: str, index: int, value: str):
        """
        Set an item in a list.
        :param key: The key of the list.
        :param index: The index of the item.
        :param value: The value to set.
        """
        return self.request("LIST_SET", key, index, value)

    def list_push_left(self, key: str, list: list):
        """
        Push items to the left of a list.
        :param key: The key of the list.
        :param list: The items to push.
        :return: The number of items in the list.
        """
        return self.request("LIST_PUSH_LEFT", key, list)

    def list_push_right(self, key: str, list: list):
        """
        Push items to the right of a list.
        :param key: The key of the list.
        :param list: The items to push.
        :return: The number of items in the list.
        """
        return self.request("LIST_PUSH_RIGHT", key, list)

    def list_pop_left(self, key: str, count: int = 1):
        """
        Pop items from the left of a list.
        :param key: The key of the list.
        :param count: The number of items to pop.
        :return: The popped items.
        """
        return self.request("LIST_POP_LEFT", key, count)

    def list_pop_right(self, key: str, count: int = 1):
        """
        Pop items from the right of a list.
        :param key: The key of the list.
        :param count: The number of items to pop.
        :return: The popped items.
        """
        return self.request("LIST_POP_RIGHT", key, count)

    def list_range(self, key: str, start: int, end: int):
        """
        Get a range of items from a list.
        :param key: The key of the list.
        :param start: The start index.
        :param end: The end index.
        :return: The items in the range.
        """
        return self.request("LIST_RANGE", key, start, end)

    def list_trim(self, key: str, start: int, end: int):
        """
        Trim a list.
        :param key: The key of the list.
        :param start: The start index.
        :param end: The end index.
        :return: The number of items trimmed.
        """
        return self.request("LIST_TRIM", key, start, end)

    def list_remove(self, key: str, value: str, count: int = 1):
        """
        Remove items from a list.
        :param key: The key of the list.
        :param value: The value to remove.
        :param count: The number of items to remove.
        :return: The number of items removed.
        """
        return self.request("LIST_REMOVE", key, value, count)

    def set_add(self, key: str, set: set):
        """
        Add items to a set.
        :param key: The key of the set.
        :param set: The items to add.
        :return: The number of items added.
        """
        return self.request("SET_ADD", key, set)

    def set_all(self, key: str):
        """
        Get all items from a set.
        :param key: The key of the set.
        :return: The items in the set.
        """
        return self.request("SET_ALL", key)

    def set_remove(self, key: str, count: int = 1):
        """
        Randomly remove items from a set
        :param key: The key of the set.
        :param count: The number of items to remove.
        :return: The items removed.
        """
        return self.request("SET_REMOVE", key, count)

    def set_discard(self, key: str, set: set):
        """
        Randomly discard items from a set
        :param key: The key of the set.
        :param set: The items to discard.
        """
        return self.request("SET_DISCARD", key, set)

    def set_len(self, key: str):
        """
        Get the length of a set.
        :param key: The key of the set.
        :return: The length of the set.
        """
        return self.request("SET_LEN", key)

    def set_contains(self, key: str, value: str):
        """
        Check if a set contains a value.
        :param key: The key of the set.
        :param value: The value to check.
        :return: Whether the set contains the value.
        """
        return self.request("SET_CONTAINS", key, value)

    def set_move(self, source: str, dest: str, value: str):
        """
        Move a value from one set to another.
        :param source: The key of the source set.
        :param dest: The key of the destination set.
        :param value: The value to move.
        :return: Whether the value was moved.
        """
        return self.request("SET_MOVE", source, dest, value)

    def set_union(self, *keys):
        """
        Get the union of many sets.
        :param keys: The keys of the sets.
        :return: The union of the sets.
        """
        return self.request("SET_UNION", list(keys))

    def set_union_into(self, dest: str, *keys):
        """
        Get the union of many sets and store it in another set.
        :param dest: The key of the destination set.
        :param keys: The keys of the sets.
        :return: The length of the union of the sets.
        """
        return self.request("SET_UNION_INTO", dest, list(keys))

    def set_union_len(self, *keys):
        """
        Get the length of the union of many sets.
        :param keys: The keys of the sets.
        :return: The length of the union of the sets.
        """
        return self.request("SET_UNION_LEN", list(keys))

    def set_intersect(self, *keys):
        """
        Get the intersection of many sets.
        :param keys: The keys of the sets.
        :return: The intersection of the sets.
        """
        return self.request("SET_INTERSECT", list(keys))

    def set_intersect_into(self, dest: str, *keys):
        """
        Get the intersection of many sets and store it in another set.
        :param dest: The key of the destination set.
        :param keys: The keys of the sets.
        :return: The length of the intersection of the sets.
        """
        return self.request("SET_INTERSECT_INTO", dest, list(keys))

    def set_intersect_len(self, *keys):
        """
        Get the length of the intersection of many sets.
        :param keys: The keys of the sets.
        :return: The length of the intersection of the sets.
        """
        return self.request("SET_INTERSECT_LEN", list(keys))

    def set_diff(self, key1: str, key2: str):
        """
        Get the difference of two sets.
        :param key1: The key of the first set.
        :param key2: The key of the second set.
        :return: The difference of the sets.
        """
        return self.request("SET_DIFF", key1, key2)

    def set_diff_into(self, dest: str, key1: str, key2: str):
        """
        Get the difference of two sets and store it in another set.
        :param dest: The key of the destination set.
        :param key1: The key of the first set.
        :param key2: The key of the second set.
        :return: The length of the difference of the sets.
        """
        return self.request("SET_DIFF_INTO", dest, key1, key2)

    def set_diff_len(self, key1: str, key2: str):
        """
        Get the length of the difference of two sets.
        :param key1: The key of the first set.
        :param key2: The key of the second set.
        :return: The length of the difference of the sets.
        """
        return self.request("SET_DIFF_LEN", key1, key2)

    def hash_set(self, key: str, values: dict[str, str]):
        """
        Set the value of a key to a hash with the contents of values
        :param key: the key to store the hash
        :param values: the dict containing the data to store in the hash
        """
        return self.request("HASH_SET", key, values)

    def hash_all(self, key: str):
        """
        Get all the fields and values of a hash
        :param key: the key of the hash
        """
        return self.request("HASH_ALL", key)

    def hash_get(self, key: str, field: str):
        """
        Get the value of a field in a hash
        :param key: the key of the hash
        :param field: the field to get the value of
        """
        return self.request("HASH_GET", key, field)

    def hash_contains(self, key: str, field: str):
        """
        Check if a hash contains a field
        :param key: the key of the hash
        :param field: the field to check
        """
        return self.request("HASH_CONTAINS", key, field)

    def hash_len(self, key: str):
        """
        Get the number of fields in a hash
        :param key: the key of the hash
        """
        return self.request("HASH_LEN", key)

    def hash_key_len(self, key: str, field: str):
        """
        Get the length of a field in a hash
        :param key: the key of the hash
        :param field: the field to get the length of
        """
        return self.request("HASH_KEY_LEN", key, field)

    def hash_remove(self, key: str, fields: list[str]):
        """
        Remove fields from a hash
        :param key: the key of the hash
        :param fields: the fields to remove
        """
        return self.request("HASH_REMOVE", key, fields)

    def hash_keys(self, key: str):
        """
        Get the keys of a hash
        :param key: the key of the hash
        """
        return self.request("HASH_KEYS", key)

    def hash_values(self, key: str):
        """
        Get the values of a hash
        :param key: the key of the hash
        """
        return self.request("HASH_VALUES", key)

    def hash_many_get(self, key: str, fields: list[str]):
        """
        Get many fields from a hash
        :param key: the key of the hash
        :param fields: the fields to get
        """
        return self.request("HASH_MANY_GET", key, fields)

    def transact_begin(self):
        """
        Begin a transaction
        """
        return self.request("TRANSACT_BEGIN")

    def transact_commit(self):
        """
        Commit a transaction
        """
        return self.request("TRANSACT_COMMIT")

    def transact_discard(self):
        """
        Discard a transaction
        """
        return self.request("TRANSACT_DISCARD")

    def publish(self, channel: str, message: str):
        """
        Publish a message to a channel
        :param channel: the channel to publish to
        :param message: the message to publish
        """
        return self.request("PUBLISH", channel, message)

    def subscribe(self, channel: str):
        """
        Subscribe to a channel
        :param channel: the channel to subscribe to
        """
        return self.request("SUBSCRIBE", channel)

    def subscribed(self):
        """
        Get the channels subscribed to
        """
        return self.request("SUBSCRIBED")

    def unsubscribe(self, channel: str):
        """
        Unsubscribe from a channel
        :param channel: the channel to unsubscribe from
        """
        return self.request("UNSUBSCRIBE", channel)

    def unsubscribe_all(self):
        """
        Unsubscribe from all channels
        """
        return self.request("UNSUBSCRIBE_ALL")

    def cluster_key(self):
        """
        Get the key of a cluster
        """
        return self.request("CLUSTER_KEY")

    def cluster_leave(self):
        """
        Leave a cluster
        :param key: the key of the cluster
        """
        return self.request("CLUSTER_LEAVE")

    def cluster_new(self, key: str):
        """
        Create a new cluster
        :param key: the key of the cluster
        """
        return self.request("CLUSTER_NEW", key)

    def peers(self):
        """
        Get the peers in a cluster
        """
        return self.request("PEERS")

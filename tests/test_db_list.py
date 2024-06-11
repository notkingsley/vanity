from tests.test_db import BaseDatabaseTest


class NonExistentDatabaseKeyTest(BaseDatabaseTest):
    """
    Test that a non-existent database key behaves like an empty list.
    """

    def test_non_existent_list_len(self):
        """
        Test that the length of a non-existent list is 0.
        """
        response = self.client.list_len("non-existent-list")
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 0)

    def test_non_existent_list_get(self):
        """
        Test that getting an item from a non-existent list returns an out-of-range error.
        """
        response = self.client.list_get("non-existent-list", 0)
        self.assertTrue(response.is_error())
        self.assertEqual(response.value, "index out of range")

    def test_non_existent_list_set(self):
        """
        Test that setting an item in a non-existent list returns an out-of-range error.
        """
        response = self.client.list_set("non-existent-list", 0, "value")
        self.assertTrue(response.is_error())
        self.assertEqual(response.value, "index out of range")

    def test_non_existent_list_push_left(self):
        """
        Test that pushing an item to the left of a non-existent list creates the list.
        """
        response = self.client.list_push_left("non-existent-list", ["value"])
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 1)

    def test_non_existent_list_push_right(self):
        """
        Test that pushing an item to the right of a non-existent list creates the list.
        """
        response = self.client.list_push_right("non-existent-list", ["value"])
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 1)

    def test_non_existent_list_pop_left(self):
        """
        Test that popping an item from the left of a non-existent list return an empty list.
        """
        response = self.client.list_pop_left("non-existent-list", 10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, [])

    def test_non_existent_list_pop_right(self):
        """
        Test that popping an item from the right of a non-existent list return an empty list.
        """
        response = self.client.list_pop_right("non-existent-list", 10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, [])

    def test_non_existent_list_range(self):
        """
        Test that getting a range from a non-existent list returns an empty list.
        """
        response = self.client.list_range("non-existent-list", 0, 10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, [])

    def test_non_existent_list_trim(self):
        """
        Test that trimming a non-existent list returns 0
        """
        response = self.client.list_trim("non-existent-list", 0, 10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 0)

    def test_non_existent_list_remove(self):
        """
        Test that removing an item from a non-existent list returns 0
        """
        response = self.client.list_remove("non-existent-list", "value")
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 0)


class WrongDatabaseKeyTypeTest(BaseDatabaseTest):
    """
    Test the behaviour of list commands on database keys that are not lists.
    """

    def test_wrong_type_list_len(self):
        """
        Test that getting the length of a list key that is not a list returns an error.
        """
        response = self.client.str_set("not-a-list", "value")
        self.assertTrue(response.is_ok())
        response = self.client.list_len("not-a-list")
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a list")

    def test_wrong_type_list_get(self):
        """
        Test that getting an item from a list key that is not a list returns an error.
        """
        response = self.client.str_set("not-a-list", "value")
        self.assertTrue(response.is_ok())
        response = self.client.list_get("not-a-list", 0)
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a list")

    def test_wrong_type_list_set(self):
        """
        Test that setting an item in a list key that is not a list returns an error.
        """
        response = self.client.str_set("not-a-list", "value")
        self.assertTrue(response.is_ok())
        response = self.client.list_set("not-a-list", 0, "value")
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a list")

    def test_wrong_type_list_push_left(self):
        """
        Test that pushing an item to the left of a list key that is not a list returns an error.
        """
        response = self.client.str_set("not-a-list", "value")
        self.assertTrue(response.is_ok())
        response = self.client.list_push_left("not-a-list", ["value"])
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a list")

    def test_wrong_type_list_push_right(self):
        """
        Test that pushing an item to the right of a list key that is not a list returns an error.
        """
        response = self.client.str_set("not-a-list", "value")
        self.assertTrue(response.is_ok())
        response = self.client.list_push_right("not-a-list", ["value"])
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a list")

    def test_wrong_type_list_pop_left(self):
        """
        Test that popping an item from the left of a list key that is not a list returns an error.
        """
        response = self.client.str_set("not-a-list", "value")
        self.assertTrue(response.is_ok())
        response = self.client.list_pop_left("not-a-list", 10)
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a list")

    def test_wrong_type_list_pop_right(self):
        """
        Test that popping an item from the right of a list key that is not a list returns an error.
        """
        response = self.client.str_set("not-a-list", "value")
        self.assertTrue(response.is_ok())
        response = self.client.list_pop_right("not-a-list", 10)
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a list")

    def test_wrong_type_list_range(self):
        """
        Test that getting a range from a list key that is not a list returns an error.
        """
        response = self.client.str_set("not-a-list", "value")
        self.assertTrue(response.is_ok())
        response = self.client.list_range("not-a-list", 0, 10)
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a list")

    def test_wrong_type_list_trim(self):
        """
        Test that trimming a list key that is not a list returns an error.
        """
        response = self.client.str_set("not-a-list", "value")
        self.assertTrue(response.is_ok())
        response = self.client.list_trim("not-a-list", 0, 10)
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a list")

    def test_wrong_type_list_remove(self):
        """
        Test that removing an item from a list key that is not a list returns an error.
        """
        response = self.client.str_set("not-a-list", "value")
        self.assertTrue(response.is_ok())
        response = self.client.list_remove("not-a-list", "value")
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a list")


class ListMethodsTest(BaseDatabaseTest):
    """
    Test the behaviour of list methods.
    """

    def setUp(self) -> None:
        super().setUp()
        self.client.list_push_right(
            "test_list_methods",
            [
                "value1",
                "value2",
                "value3",
                "value4",
                "value5",
            ],
        )

    def test_list_methods(self):
        """
        Test that the length of a list is correct.
        """
        response = self.client.list_len("test_list_methods")
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 5)

    def test_list_get(self):
        """
        Test that getting an item from a list is correct.
        """
        response = self.client.list_get("test_list_methods", 2)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value3")

    def test_list_set(self):
        """
        Test that setting an item in a list is correct.
        """
        response = self.client.list_set("test_list_methods", 2, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_get("test_list_methods", 2)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "valuex")

    def test_list_push_left(self):
        """
        Test that pushing an item to the left of a list is correct.
        """
        response = self.client.list_push_left(
            "test_list_methods", ["value0", "value-1", "value-2"]
        )
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 8)
        response = self.client.list_range("test_list_methods", 0, -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            [
                "value-2",
                "value-1",
                "value0",
                "value1",
                "value2",
                "value3",
                "value4",
                "value5",
            ],
        )

    def test_list_push_right(self):
        """
        Test that pushing an item to the right of a list is correct.
        """
        response = self.client.list_push_right(
            "test_list_methods", ["value6", "value7", "value8"]
        )
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 8)
        response = self.client.list_range("test_list_methods", 0, -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            [
                "value1",
                "value2",
                "value3",
                "value4",
                "value5",
                "value6",
                "value7",
                "value8",
            ],
        )

    def test_list_pop_left(self):
        """
        Test that popping an item from the left of a list is correct.
        """
        response = self.client.list_pop_left("test_list_methods", 2)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, ["value1", "value2"])
        response = self.client.list_len("test_list_methods")
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 3)

    def test_list_pop_right(self):
        """
        Test that popping an item from the right of a list is correct.
        """
        response = self.client.list_pop_right("test_list_methods", 2)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, ["value4", "value5"])
        response = self.client.list_len("test_list_methods")
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 3)

    def test_list_range(self):
        """
        Test that getting a range from a list is correct.
        """
        response = self.client.list_range("test_list_methods", 1, 3)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, ["value2", "value3", "value4"])

    def test_list_trim(self):
        """
        Test that trimming a list is correct.
        """
        response = self.client.list_trim("test_list_methods", 1, 3)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 2)
        response = self.client.list_range("test_list_methods", 0, -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, ["value2", "value3", "value4"])

    def test_list_remove(self):
        """
        Test that removing an item from a list is correct.
        """
        response = self.client.list_remove("test_list_methods", "value3")
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 1)
        response = self.client.list_range("test_list_methods", 0, -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, ["value1", "value2", "value4", "value5"])

    def test_list_remove_multiple(self):
        """
        Test that removing multiple items from a list is correct.
        """
        self.client.list_set("test_list_methods", 0, "valuex")
        self.client.list_set("test_list_methods", 2, "valuex")
        response = self.client.list_remove("test_list_methods", "valuex", 2)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 2)
        response = self.client.list_range("test_list_methods", 0, -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, ["value2", "value4", "value5"])


class NegativeIndicesTest(BaseDatabaseTest):
    """
    Test that negative indices are handled correctly.
    """

    def setUp(self) -> None:
        super().setUp()
        self.client.list_push_right(
            "test_list_negative_indices",
            [
                "value1",
                "value2",
                "value3",
                "value4",
                "value5",
                "value6",
                "value7",
                "value8",
                "value9",
                "value10",
                "value11",
                "value12",
            ],
        )

    def test_list_get(self):
        """
        Test that getting an item from a list with a negative index is correct.
        """
        response = self.client.list_get("test_list_negative_indices", -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value12")
        response = self.client.list_get("test_list_negative_indices", -2)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value11")
        response = self.client.list_get("test_list_negative_indices", -3)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value10")
        response = self.client.list_get("test_list_negative_indices", -4)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value9")
        response = self.client.list_get("test_list_negative_indices", -5)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value8")
        response = self.client.list_get("test_list_negative_indices", -6)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value7")
        response = self.client.list_get("test_list_negative_indices", -7)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value6")
        response = self.client.list_get("test_list_negative_indices", -8)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value5")
        response = self.client.list_get("test_list_negative_indices", -9)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value4")
        response = self.client.list_get("test_list_negative_indices", -10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value3")
        response = self.client.list_get("test_list_negative_indices", -11)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value2")
        response = self.client.list_get("test_list_negative_indices", -12)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "value1")

    def test_list_set(self):
        """
        Test that setting an item in a list with a negative index is correct.
        """
        response = self.client.list_set("test_list_negative_indices", -3, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_get("test_list_negative_indices", -3)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "valuex")
        response = self.client.list_set("test_list_negative_indices", -7, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_get("test_list_negative_indices", -7)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "valuex")

    def test_list_pop_left(self):
        """
        Test that popping an item from the left of a list with negative indices is correct.
        """
        response = self.client.list_pop_left("test_list_negative_indices", -7)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value, ["value1", "value2", "value3", "value4", "value5"]
        )
        response = self.client.list_len("test_list_negative_indices")
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 7)

    def test_list_pop_right(self):
        """
        Test that popping an item from the right of a list with negative indices is correct.
        """
        response = self.client.list_pop_right("test_list_negative_indices", -7)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value, ["value8", "value9", "value10", "value11", "value12"]
        )
        response = self.client.list_len("test_list_negative_indices")
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 7)

    def test_list_range_negative_start(self):
        """
        Test that getting a range from a list with a negative start index is correct.
        """
        response = self.client.list_range("test_list_negative_indices", -7, 12)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            ["value6", "value7", "value8", "value9", "value10", "value11", "value12"],
        )

    def test_list_range_negative_end(self):
        """
        Test that getting a range from a list with a negative end index is correct.
        """
        response = self.client.list_range("test_list_negative_indices", 0, -2)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            [
                "value1",
                "value2",
                "value3",
                "value4",
                "value5",
                "value6",
                "value7",
                "value8",
                "value9",
                "value10",
                "value11",
            ],
        )

    def test_list_range_negative_start_and_end(self):
        """
        Test that getting a range from a list with negative start and end indices is correct.
        """
        response = self.client.list_range("test_list_negative_indices", -7, -2)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            ["value6", "value7", "value8", "value9", "value10", "value11"],
        )

    def test_invalid_list_range(self):
        """
        Test that getting a range from a list with an invalid range returns an empty list
        """
        response = self.client.list_range("test_list_negative_indices", 5, 2)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, [])

    def test_invalid_list_range_negative_start(self):
        """
        Test that getting a range from a list with an invalid range and negative start returns an empty list
        """
        response = self.client.list_range("test_list_negative_indices", -7, -10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, [])

    def test_invalid_list_range_negative_end(self):
        """
        Test that getting a range from a list with an invalid range and negative end returns an empty list
        """
        response = self.client.list_range("test_list_negative_indices", -2, -10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, [])

    def test_invalid_list_range_negative_start_and_end(self):
        """
        Test that getting a range from a list with an invalid range and negative start and end returns an empty list
        """
        response = self.client.list_range("test_list_negative_indices", -7, -10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, [])

    def test_list_trim_negative_start(self):
        """
        Test that trimming a list with a negative start index is correct.
        """
        response = self.client.list_trim("test_list_negative_indices", -7, 12)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 5)
        response = self.client.list_range("test_list_negative_indices", 0, -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            ["value6", "value7", "value8", "value9", "value10", "value11", "value12"],
        )

    def test_list_trim_negative_end(self):
        """
        Test that trimming a list with a negative end index is correct.
        """
        response = self.client.list_trim("test_list_negative_indices", 0, -7)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 6)
        response = self.client.list_range("test_list_negative_indices", 0, -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value, ["value1", "value2", "value3", "value4", "value5", "value6"]
        )

    def test_list_trim_negative_start_and_end(self):
        """
        Test that trimming a list with negative start and end indices is correct.
        """
        response = self.client.list_trim("test_list_negative_indices", -7, -2)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 6)
        response = self.client.list_range("test_list_negative_indices", 0, -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            ["value6", "value7", "value8", "value9", "value10", "value11"],
        )

    def test_invalid_list_trim(self):
        """
        Test that trimming a list with an invalid range returns 0
        """
        response = self.client.list_trim("test_list_negative_indices", 5, 2)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 0)

    def test_invalid_list_trim_negative_start(self):
        """
        Test that trimming a list with an invalid range and negative start returns 0
        """
        response = self.client.list_trim("test_list_negative_indices", -7, -10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 0)

    def test_invalid_list_trim_negative_end(self):
        """
        Test that trimming a list with an invalid range and negative end returns 0
        """
        response = self.client.list_trim("test_list_negative_indices", -2, -10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 0)

    def test_invalid_list_trim_negative_start_and_end(self):
        """
        Test that trimming a list with an invalid range and negative start and end returns 0
        """
        response = self.client.list_trim("test_list_negative_indices", -7, -10)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 0)

    def test_list_remove_negative(self):
        """
        Test that removing an item from a list with negative indices removes from the end of the list.
        """
        response = self.client.list_set("test_list_negative_indices", 1, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_set("test_list_negative_indices", 4, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_set("test_list_negative_indices", 6, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_set("test_list_negative_indices", 9, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_remove("test_list_negative_indices", "valuex", -3)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 3)
        response = self.client.list_range("test_list_negative_indices", 0, -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            [
                "value1",
                "valuex",
                "value3",
                "value4",
                "value6",
                "value8",
                "value9",
                "value11",
                "value12",
            ],
        )

    def test_list_remove_negative_zero(self):
        """
        Test that removing an item from a list with zero count removes all items.
        """
        response = self.client.list_set("test_list_negative_indices", 1, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_set("test_list_negative_indices", 4, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_set("test_list_negative_indices", 6, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_set("test_list_negative_indices", 9, "valuex")
        self.assertTrue(response.is_ok())
        response = self.client.list_remove("test_list_negative_indices", "valuex", 0)
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 4)
        response = self.client.list_range("test_list_negative_indices", 0, -1)
        self.assertTrue(response.is_ok())
        self.assertEqual(
            response.value,
            [
                "value1",
                "value3",
                "value4",
                "value6",
                "value8",
                "value9",
                "value11",
                "value12",
            ],
        )

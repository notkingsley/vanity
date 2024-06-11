from tests.test_db import BaseDatabaseTest


class NonExistentDatabaseKeyTest(BaseDatabaseTest):
    """
    Test that a non-existent key behaves like an empty set
    """

    def test_set_add(self):
        """
        Test that we can add to a non-existent key
        """
        response = self.client.set_add("test_set_add", {"test_set_add_value"})
        self.assertEqual(response.value, 1)
        response = self.client.set_all("test_set_add")
        self.assertEqual(response.value, {"test_set_add_value"})

    def test_set_all(self):
        """
        Test that we can get all from a non-existent key
        """
        response = self.client.set_all("test_set_all")
        self.assertEqual(response.value, set())

    def test_set_remove(self):
        """
        Test that we can remove from a non-existent key
        """
        response = self.client.set_remove("test_set_remove", 3)
        self.assertEqual(response.value, set())

    def test_set_discard(self):
        """
        Test that we can discard from a non-existent key
        """
        response = self.client.set_discard(
            "test_set_discard", {"test_set_discard_value"}
        )
        self.assertEqual(response.value, 0)

    def test_set_len(self):
        """
        Test that we can get the length of a non-existent key
        """
        response = self.client.set_len("test_set_len")
        self.assertEqual(response.value, 0)

    def test_set_contains(self):
        """
        Test that we can check if a value is in a non-existent key
        """
        response = self.client.set_contains(
            "test_set_contains", "test_set_contains_value"
        )
        self.assertEqual(response.value, False)

    def test_set_move(self):
        """
        Test that we can move a value from a non-existent key
        """
        response = self.client.set_move(
            "test_set_move", "test_set_move_destination", "test_set_move_value"
        )
        self.assertEqual(response.value, False)

    def test_set_union(self):
        """
        Test that we can union a non-existent key
        """
        response = self.client.set_union(
            "test_set_union1", "test_set_union2", "test_set_union3"
        )
        self.assertEqual(response.value, set())

    def test_set_union_into(self):
        """
        Test that we can union a non-existent key into an existing key
        """
        response = self.client.set_union_into(
            "test_set_union_into_dest",
            "test_set_union_into1",
            "test_set_union_into2",
            "test_set_union_into3",
        )
        self.assertEqual(response.value, 0)
        response = self.client.set_all("test_set_union_into_dest")
        self.assertEqual(response.value, set())

    def test_set_uniom_len(self):
        """
        Test that we can union a non-existent key
        """
        response = self.client.set_union_len(
            "test_set_union_len1", "test_set_union_len2", "test_set_union_len3"
        )
        self.assertEqual(response.value, 0)

    def test_set_intersect(self):
        """
        Test that we can intersect a non-existent key
        """
        response = self.client.set_intersect(
            "test_set_intersect1", "test_set_intersect2", "test_set_intersect3"
        )
        self.assertEqual(response.value, set())

    def test_set_intersect_into(self):
        """
        Test that we can intersect a non-existent key into an existing key
        """
        response = self.client.set_intersect_into(
            "test_set_intersect_into_dest",
            "test_set_intersect_into1",
            "test_set_intersect_into2",
            "test_set_intersect_into3",
        )
        self.assertEqual(response.value, 0)
        response = self.client.set_all("test_set_intersect_into_dest")
        self.assertEqual(response.value, set())

    def test_set_intersect_len(self):
        """
        Test that we can intersect a non-existent key
        """
        response = self.client.set_intersect_len(
            "test_set_intersect_len1",
            "test_set_intersect_len2",
            "test_set_intersect_len3",
        )
        self.assertEqual(response.value, 0)

    def test_set_diff(self):
        """
        Test that we can difference two non-existent keys
        """
        response = self.client.set_diff("test_set_diff1", "test_set_diff2")
        self.assertEqual(response.value, set())

    def test_set_diff_into(self):
        """
        Test that we can difference two non-existent keys into an existing key
        """
        response = self.client.set_diff_into(
            "test_set_diff_into_dest", "test_set_diff_into1", "test_set_diff_into2"
        )
        self.assertEqual(response.value, 0)
        response = self.client.set_all("test_set_diff_into_dest")
        self.assertEqual(response.value, set())

    def test_set_diff_len(self):
        """
        Test that we can difference two non-existent keys
        """
        response = self.client.set_diff_len("test_set_diff_len1", "test_set_diff_len2")
        self.assertEqual(response.value, 0)


class SetBadTypeTest(BaseDatabaseTest):
    """
    Test set_ operations on a key with a bad type
    """

    def setUp(self) -> None:
        super().setUp()
        response = self.client.str_set("test_set_bad_type", "test_set_bad_type_value")
        self.assertTrue(response.is_ok())

    def test_set_add(self):
        """
        Test that we can't add to a key with a bad type
        """
        response = self.client.set_add("test_set_bad_type", {"test_set_bad_type_value"})
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a set")

    def test_set_all(self):
        """
        Test that we can't get all from a key with a bad type
        """
        response = self.client.set_all("test_set_bad_type")
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a set")

    def test_set_remove(self):
        """
        Test that we can't remove from a key with a bad type
        """
        response = self.client.set_remove("test_set_bad_type", 3)
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a set")

    def test_set_discard(self):
        """
        Test that we can't discard from a key with a bad type
        """
        response = self.client.set_discard(
            "test_set_bad_type", {"test_set_bad_type_value"}
        )
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a set")

    def test_set_len(self):
        """
        Test that we can't get the length of a key with a bad type
        """
        response = self.client.set_len("test_set_bad_type")
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a set")

    def test_set_contains(self):
        """
        Test that we can't check if a value is in a key with a bad type
        """
        response = self.client.set_contains(
            "test_set_bad_type", "test_set_bad_type_value"
        )
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a set")

    def test_set_move(self):
        """
        Test that we can't move a value from a key with a bad type
        """
        response = self.client.set_move(
            "test_set_bad_type", "test_set_move_destination", "test_set_move_value"
        )
        self.assertTrue(response.is_bad_type())
        self.assertEqual(response.value, "not a set")


class SetTest(BaseDatabaseTest):
    """
    Test set_ operations
    """

    def setUp(self) -> None:
        response = self.client.set_add(
            "set_test", {"set_test_value1", "set_test_value2", "set_test_value3"}
        )
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, 3)

    def test_set_add(self):
        """
        Test that we can add to a set
        """
        response = self.client.set_add("set_test", {"test_set_add_value"})
        self.assertEqual(response.value, 1)
        response = self.client.set_contains("set_test", "test_set_add_value")
        self.assertEqual(response.value, True)

    def test_set_all(self):
        """
        Test that we can get all from a set
        """
        response = self.client.set_all("set_test")
        self.assertEqual(
            response.value, {"set_test_value1", "set_test_value2", "set_test_value3"}
        )

    def test_set_remove(self):
        """
        Test that we can remove from a set
        """
        response = self.client.set_remove("set_test", 2)
        self.assertEqual(len(response.value), 2)
        response = self.client.set_len("set_test")
        self.assertEqual(response.value, 1)

    def test_set_discard(self):
        """
        Test that we can discard from a set
        """
        response = self.client.set_discard(
            "set_test", {"set_test_value1", "set_test_value2"}
        )
        self.assertEqual(response.value, 2)
        response = self.client.set_all("set_test")
        self.assertEqual(response.value, {"set_test_value3"})

    def test_set_len(self):
        """
        Test that we can get the length of a set
        """
        response = self.client.set_len("set_test")
        self.assertEqual(response.value, 3)

    def test_set_contains(self):
        """
        Test that we can check if a value is in a set
        """
        response = self.client.set_contains("set_test", "set_test_value1")
        self.assertEqual(response.value, True)
        response = self.client.set_contains("set_test", "set_test_value4")
        self.assertEqual(response.value, False)

    def test_set_move(self):
        """
        Test that we can move a value from a set
        """
        response = self.client.set_move(
            "set_test", "set_test_move_destination", "set_test_value1"
        )
        self.assertEqual(response.value, True)
        response = self.client.set_contains("set_test", "set_test_value1")
        self.assertEqual(response.value, False)
        response = self.client.set_contains(
            "set_test_move_destination", "set_test_value1"
        )
        self.assertEqual(response.value, True)

    def test_set_union(self):
        """
        Test that we can union two sets
        """
        response = self.client.set_add(
            "set_test_union1", {"set_test_union1_value1", "set_test_union1_value2"}
        )
        self.assertEqual(response.value, 2)
        response = self.client.set_add(
            "set_test_union2", {"set_test_union2_value1", "set_test_union2_value2"}
        )
        self.assertEqual(response.value, 2)
        response = self.client.set_union(
            "set_test", "set_test_union1", "set_test_union2"
        )
        self.assertEqual(
            response.value,
            {
                "set_test_value1",
                "set_test_value2",
                "set_test_value3",
                "set_test_union1_value1",
                "set_test_union1_value2",
                "set_test_union2_value1",
                "set_test_union2_value2",
            },
        )

    def test_set_union_into(self):
        """
        Test that we can union two sets into an existing set
        """
        response = self.client.set_add(
            "set_test_union_into1",
            {"set_test_union_into1_value1", "set_test_union_into1_value2"},
        )
        self.assertEqual(response.value, 2)
        response = self.client.set_add(
            "set_test_union_into2",
            {"set_test_union_into2_value1", "set_test_union_into2_value2"},
        )
        self.assertEqual(response.value, 2)
        response = self.client.set_union_into(
            "set_test", "set_test", "set_test_union_into1", "set_test_union_into2"
        )
        self.assertEqual(response.value, 7)
        response = self.client.set_all("set_test")
        self.assertEqual(
            response.value,
            {
                "set_test_value1",
                "set_test_value2",
                "set_test_value3",
                "set_test_union_into1_value1",
                "set_test_union_into1_value2",
                "set_test_union_into2_value1",
                "set_test_union_into2_value2",
            },
        )

    def test_set_union_len(self):
        """
        Test that we can union two sets
        """
        response = self.client.set_add(
            "set_test_union_len1",
            {"set_test_union_len1_value1", "set_test_union_len1_value2"},
        )
        self.assertEqual(response.value, 2)
        response = self.client.set_add(
            "set_test_union_len2",
            {"set_test_union_len2_value1", "set_test_union_len2_value2"},
        )
        self.assertEqual(response.value, 2)
        response = self.client.set_union_len(
            "set_test", "set_test_union_len1", "set_test_union_len2"
        )
        self.assertEqual(response.value, 7)

    def test_set_intersect(self):
        """
        Test that we can intersect two sets
        """
        response = self.client.set_add(
            "set_test_intersect1",
            {
                "set_test_intersect1_value1",
                "set_test_intersect1_value2",
                "set_test_intersect_common",
            },
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_add(
            "set_test_intersect2",
            {
                "set_test_intersect2_value1",
                "set_test_intersect2_value2",
                "set_test_intersect_common",
            },
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_intersect(
            "set_test_intersect1", "set_test_intersect2"
        )
        self.assertEqual(response.value, {"set_test_intersect_common"})

    def test_set_intersect_into(self):
        """
        Test that we can intersect two sets into an existing set
        """
        response = self.client.set_add(
            "set_test_intersect_into1",
            {
                "set_test_intersect_into1_value1",
                "set_test_intersect_into1_value2",
                "set_test_intersect_into_common",
            },
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_add(
            "set_test_intersect_into2",
            {
                "set_test_intersect_into2_value1",
                "set_test_intersect_into2_value2",
                "set_test_intersect_into_common",
            },
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_intersect_into(
            "set_test", "set_test_intersect_into1", "set_test_intersect_into2"
        )
        self.assertEqual(response.value, 1)
        response = self.client.set_all("set_test")
        self.assertEqual(response.value, {"set_test_intersect_into_common"})

    def test_set_intersect_len(self):
        """
        Test that we can intersect two sets
        """
        response = self.client.set_add(
            "set_test_intersect_len1",
            {
                "set_test_intersect_len1_value1",
                "set_test_intersect_len1_value2",
                "set_test_intersect_len_common",
            },
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_add(
            "set_test_intersect_len2",
            {
                "set_test_intersect_len2_value1",
                "set_test_intersect_len2_value2",
                "set_test_intersect_len_common",
            },
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_intersect_len(
            "set_test_intersect_len1", "set_test_intersect_len2"
        )
        self.assertEqual(response.value, 1)

    def test_set_diff(self):
        """
        Test that we can difference two sets
        """
        response = self.client.set_add(
            "set_test_diff1",
            {"set_test_diff1_value1", "set_test_diff1_value2", "set_test_diff_common"},
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_add(
            "set_test_diff2",
            {"set_test_diff2_value1", "set_test_diff2_value2", "set_test_diff_common"},
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_diff("set_test_diff1", "set_test_diff2")
        self.assertEqual(
            response.value, {"set_test_diff1_value1", "set_test_diff1_value2"}
        )

    def test_set_diff_into(self):
        """
        Test that we can difference two sets into an existing set
        """
        response = self.client.set_add(
            "set_test_diff_into1",
            {
                "set_test_diff_into1_value1",
                "set_test_diff_into1_value2",
                "set_test_diff_into_common",
            },
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_add(
            "set_test_diff_into2",
            {
                "set_test_diff_into2_value1",
                "set_test_diff_into2_value2",
                "set_test_diff_into_common",
            },
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_diff_into(
            "set_test", "set_test_diff_into1", "set_test_diff_into2"
        )
        self.assertEqual(response.value, 2)
        response = self.client.set_all("set_test")
        self.assertEqual(
            response.value, {"set_test_diff_into1_value1", "set_test_diff_into1_value2"}
        )

    def test_set_diff_len(self):
        """
        Test that we can difference two sets
        """
        response = self.client.set_add(
            "set_test_diff_len1",
            {
                "set_test_diff_len1_value1",
                "set_test_diff_len1_value2",
                "set_test_diff_len_common",
            },
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_add(
            "set_test_diff_len2",
            {
                "set_test_diff_len2_value1",
                "set_test_diff_len2_value2",
                "set_test_diff_len_common",
            },
        )
        self.assertEqual(response.value, 3)
        response = self.client.set_diff_len("set_test_diff_len1", "set_test_diff_len2")
        self.assertEqual(response.value, 2)

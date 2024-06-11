import unittest
from tempfile import TemporaryDirectory

from client import AuthLevel, ServerHandle
from tests.utils import get_free_port, make_client


class UnknownUserTest(unittest.TestCase):
    """
    Test no authentication
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.port = get_free_port()
        cls.server_handle = ServerHandle(ports=[cls.port])
        cls.server_handle.start()

    @classmethod
    def tearDownClass(cls) -> None:
        cls.server_handle.stop()

    def setUp(self) -> None:
        self.client = make_client(self.port, no_login=True)

    def tearDown(self) -> None:
        self.client.close()

    def test_get_no_login(self):
        """
        Test that we can't get data from the server if we don't login.
        """
        response = self.client.get("test_get_no_login")
        self.assertTrue(response.is_denied())

    def test_set_no_login(self):
        """
        Test that we can't set data on the server if we don't login.
        """
        response = self.client.str_set("test_set_no_login", "test_set_no_login_value")
        self.assertTrue(response.is_denied())

    def test_delete_no_login(self):
        """
        Test that we can't delete data from the server if we don't login.
        """
        response = self.client.delete("test_delete_no_login")
        self.assertTrue(response.is_denied())

    def test_type_no_login(self):
        """
        Test that we can't check the type of a value if we don't login.
        """
        response = self.client.type("test_type_no_login")
        self.assertTrue(response.is_denied())

    def test_exists_no_login(self):
        """
        Test that we can't check if a key exists if we don't login.
        """
        response = self.client.exists("test_exists_no_login")
        self.assertTrue(response.is_denied())

    def test_persist_no_login(self):
        """
        Test that we can't persist data on the server if we don't login.
        """
        response = self.client.persist()
        self.assertTrue(response.is_denied())

    def test_reset_no_login(self):
        """
        Test that we can't reset the server if we don't login.
        """
        response = self.client.reset()
        self.assertTrue(response.is_denied())

    def test_terminate_no_login(self):
        """
        Test that we can't terminate the server if we don't login.
        """
        self.client.terminate()
        self.assertTrue(self.server_handle.is_running())
        response = self.client.read_response()
        self.assertTrue(response.is_denied())

    def test_ping_no_login(self):
        """
        Test that we can ping the server if we don't login.
        """
        response = self.client.ping()
        self.assertTrue(response.is_pong())

    def test_switch_db_no_login(self):
        """
        Test that we can't switch databases if we don't login.
        """
        response = self.client.switch_db(1)
        self.assertTrue(response.is_denied())

    def test_add_user_no_login(self):
        """
        Test that we can't add a user if we don't login.
        """
        response = self.client.add_user(
            "test_add_user_no_login", "test_add_user_no_login_password"
        )
        self.assertTrue(response.is_denied())

    def test_edit_user_no_login(self):
        """
        Test that we can't edit a user if we don't login.
        """
        response = self.client.edit_user("test_edit_user_no_login", AuthLevel.ADMIN)
        self.assertTrue(response.is_denied())

    def test_del_user_no_login(self):
        """
        Test that we can't delete a user if we don't login.
        """
        response = self.client.del_user("test_del_user_no_login")
        self.assertTrue(response.is_denied())

    def test_change_password_no_login(self):
        """
        Test that we can't change a user's password if we don't login.
        """
        response = self.client.change_password("test_change_password_no_login")
        self.assertTrue(response.is_denied())

    def test_check_auth_level_no_login(self):
        """
        Test that we can check the authentication level if we don't login.
        """
        response = self.client.auth_level()
        self.assertEqual(response.value, AuthLevel.UNKNOWN)

    def test_check_username_no_login(self):
        """
        Test that we can't check the username if we don't login.
        """
        response = self.client.username()
        self.assertTrue(response.is_denied())


class UserAuthTest(unittest.TestCase):
    """
    Test the authentication for a user
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.port = get_free_port()
        cls.server_handle = ServerHandle(ports=[cls.port])
        cls.server_handle.start()
        cls.admin_client = make_client(cls.port)
        cls.admin_client.add_user("test_user_auth", "test_user_auth_password")
        cls.admin_client.edit_user("test_user_auth", AuthLevel.USER)
        cls.client = make_client(cls.port, no_login=True)
        cls.client.auth("test_user_auth", "test_user_auth_password")

    @classmethod
    def tearDownClass(cls) -> None:
        cls.client.close()
        cls.admin_client.close()
        cls.server_handle.stop()

    def test_get(self):
        """
        Test that we can get data from the server if we login.
        """
        response = self.client.get("test_get")
        self.assertTrue(response.is_null())

    def test_set(self):
        """
        Test that we can set data on the server if we login.
        """
        response = self.client.str_set("test_set", "test_set_value")
        self.assertTrue(response.is_ok())
        response = self.client.get("test_set")
        self.assertTrue(response.is_ok())
        self.assertEqual(response.value, "test_set_value")

    def test_delete(self):
        """
        Test that we can delete data from the server if we login.
        """
        response = self.client.delete("test_delete")
        self.assertTrue(response.is_error())

    def test_type(self):
        """
        Test that we can check the type of a value if we login.
        """
        response = self.client.type("test_type")
        self.assertTrue(response.is_null())

    def test_exists(self):
        """
        Test that we can check if a key exists if we login.
        """
        response = self.client.exists("test_exists")
        self.assertTrue(response.is_null())

    def test_reset(self):
        """
        Test that we can't reset the server if we login.
        """
        response = self.client.reset()
        self.assertTrue(response.is_denied())

    def test_terminate(self):
        """
        Test that we can't terminate the server if we login.
        """
        self.client.terminate()
        self.assertTrue(self.server_handle.is_running())
        response = self.client.read_response()
        self.assertTrue(response.is_denied())

    def test_ping(self):
        """
        Test that we can ping the server if we login.
        """
        response = self.client.ping()
        self.assertTrue(response.is_pong())

    def test_switch_db(self):
        """
        Test that we can switch databases if we login.
        """
        response = self.client.switch_db(1)
        self.assertTrue(response.is_ok())

    def test_add_user(self):
        """
        Test that we can't add a user if we login.
        """
        response = self.client.add_user("test_add_user", "test_add_user_password")
        self.assertTrue(response.is_denied())

    def test_edit_user(self):
        """
        Test that we can't edit a user if we login.
        """
        response = self.client.edit_user("test_edit_user", AuthLevel.ADMIN)
        self.assertTrue(response.is_denied())

    def test_del_user(self):
        """
        Test that we can't delete a user if we login.
        """
        response = self.client.del_user("test_del_user")
        self.assertTrue(response.is_denied())

    def test_change_password(self):
        """
        Test that we can change the password if we login.
        """
        response = self.client.change_password("test_change_password")
        self.assertTrue(response.is_ok())
        with make_client(self.port, no_login=True) as client:
            response = client.auth("test_user_auth", "test_change_password")
            self.assertTrue(response.is_ok())

    def test_check_auth_level(self):
        """
        Test that we can check the authentication level if we login.
        """
        response = self.client.auth_level()
        self.assertEqual(response.value, AuthLevel.USER)

    def test_check_username(self):
        """
        Test that we can check the username if we login.
        """
        response = self.client.username()
        self.assertEqual(response.value, "test_user_auth")


class DefaultAuthTest(unittest.TestCase):
    """
    Test the default authentication
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.port = get_free_port()
        cls.server_handle = ServerHandle(ports=[cls.port])
        cls.server_handle.start()

    @classmethod
    def tearDownClass(cls) -> None:
        cls.server_handle.stop()

    def test_auth(self):
        """
        Test that we can authenticate with the server with the default login.
        """
        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                client.DEFAULT_ADMIN_USERNAME,
                client.DEFAULT_ADMIN_PASSWORD,
            )
            self.assertTrue(response.is_ok())

    def test_auth_fail(self):
        """
        Test that we can't authenticate with the server with the wrong password.
        """
        with make_client(self.port, no_login=True) as client:
            response = client.auth("invalid_username", "invalid_password")
            self.assertTrue(response.is_error())

    def test_default_admin(self):
        """
        Test that the default login is an admin.
        """
        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                client.DEFAULT_ADMIN_USERNAME,
                client.DEFAULT_ADMIN_PASSWORD,
            )
            self.assertTrue(response.is_ok())
            response = client.auth_level()
            self.assertEqual(response.value, AuthLevel.ADMIN)


class AccountsTest(unittest.TestCase):
    """
    Test that we can create accounts and edit their permissions.
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.port = get_free_port()
        cls.server_handle = ServerHandle(ports=[cls.port])
        cls.server_handle.start()
        cls.client = make_client(cls.port)

    @classmethod
    def tearDownClass(cls) -> None:
        cls.client.close()
        cls.server_handle.stop()

    def test_add_user(self):
        """
        Test that we can add a user.
        """
        response = self.client.add_user("test_add_user", "test_add_user_password")
        self.assertTrue(response.is_ok())
        with make_client(self.port, no_login=True) as client:
            response = client.auth("test_add_user", "test_add_user_password")
            self.assertTrue(response.is_ok())

    def test_add_user_fail(self):
        """
        Test that we can't add a user with an existing username.
        """
        response = self.client.add_user(
            "test_add_user_fail", "test_add_user_fail_password"
        )
        self.assertTrue(response.is_ok())
        response = self.client.add_user(
            "test_add_user_fail", "test_add_user_fail_password"
        )
        self.assertTrue(response.is_error())

    def test_default_add_user_is_not_admin(self):
        """
        Test that a user added with the default login is not an admin.
        """
        response = self.client.add_user(
            "test_default_add_user_is_not_admin",
            "test_default_add_user_is_not_admin_password",
        )
        self.assertTrue(response.is_ok())
        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_default_add_user_is_not_admin",
                "test_default_add_user_is_not_admin_password",
            )
            self.assertTrue(response.is_ok())
            response = client.auth_level()
            self.assertEqual(response.value, AuthLevel.USER)

    def test_add_user_fail_if_not_admin(self):
        """
        Test that we can't add a user if we're not an admin.
        """
        response = self.client.add_user(
            "test_add_user_fail_if_not_admin",
            "test_add_user_fail_if_not_admin_password",
        )
        self.assertTrue(response.is_ok())
        response = self.client.edit_user(
            "test_add_user_fail_if_not_admin", AuthLevel.USER
        )
        self.assertTrue(response.is_ok())
        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_add_user_fail_if_not_admin",
                "test_add_user_fail_if_not_admin_password",
            )
            self.assertTrue(response.is_ok())
            response = client.add_user(
                "test_add_user_fail_if_not_admin2",
                "test_add_user_fail_if_not_admin2_password",
            )
            self.assertTrue(response.is_denied())

    def test_edit_user(self):
        """
        Test that we can edit a user and make them admin.
        """
        response = self.client.add_user("test_edit_user", "test_edit_user_password")
        self.assertTrue(response.is_ok())
        response = self.client.edit_user("test_edit_user", AuthLevel.ADMIN)
        self.assertTrue(response.is_ok())
        with make_client(self.port, no_login=True) as client:
            response = client.auth("test_edit_user", "test_edit_user_password")
            self.assertTrue(response.is_ok())
            response = client.auth_level()
            self.assertEqual(response.value, AuthLevel.ADMIN)

    def test_edit_user_fail(self):
        """
        Test that we can't edit a user that doesn't exist.
        """
        response = self.client.edit_user("test_edit_user_fail", AuthLevel.ADMIN)
        self.assertTrue(response.is_error())

    def test_edit_user_fail_if_not_admin(self):
        """
        Test that we can't edit a user if we're not an admin.
        """
        response = self.client.add_user(
            "test_edit_user_fail_if_not_admin",
            "test_edit_user_fail_if_not_admin_password",
        )
        self.assertTrue(response.is_ok())
        response = self.client.edit_user(
            "test_edit_user_fail_if_not_admin", AuthLevel.USER
        )
        self.assertTrue(response.is_ok())
        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_edit_user_fail_if_not_admin",
                "test_edit_user_fail_if_not_admin_password",
            )
            self.assertTrue(response.is_ok())
            response = client.edit_user(
                "test_edit_user_fail_if_not_admin", AuthLevel.ADMIN
            )
            self.assertTrue(response.is_denied())

    def test_edit_user_auth_levels(self):
        """
        Test that we can only edit users to be USER, or ADMIN.
        """
        response = self.client.add_user(
            "test_edit_user_auth_levels", "test_edit_user_auth_levels_password"
        )
        self.assertTrue(response.is_ok())
        response = self.client.edit_user("test_edit_user_auth_levels", AuthLevel.USER)
        self.assertTrue(response.is_ok())
        response = self.client.edit_user("test_edit_user_auth_levels", AuthLevel.ADMIN)
        self.assertTrue(response.is_ok())
        response = self.client.edit_user("test_edit_user_auth_levels", AuthLevel.PEER)
        self.assertTrue(response.is_error())
        response = self.client.edit_user(
            "test_edit_user_auth_levels", AuthLevel.UNKNOWN
        )
        self.assertTrue(response.is_error())

    def test_del_user(self):
        """
        Test that we can delete a user.
        """
        response = self.client.add_user("test_del_user", "test_del_user_password")
        self.assertTrue(response.is_ok())
        response = self.client.del_user("test_del_user")
        self.assertTrue(response.is_ok())
        with make_client(self.port, no_login=True) as client:
            response = client.auth("test_del_user", "test_del_user_password")
            self.assertTrue(response.is_error())

    def test_del_user_fail(self):
        """
        Test that we can't delete a user that doesn't exist.
        """
        response = self.client.del_user("test_del_user_fail")
        self.assertTrue(response.is_error())

    def test_del_user_fail_if_not_admin(self):
        """
        Test that we can't delete a user if we're not an admin.
        """
        response = self.client.add_user(
            "test_del_user_fail_if_not_admin",
            "test_del_user_fail_if_not_admin_password",
        )
        self.assertTrue(response.is_ok())
        response = self.client.edit_user(
            "test_del_user_fail_if_not_admin", AuthLevel.USER
        )
        self.assertTrue(response.is_ok())
        response = self.client.add_user(
            "test_del_user_fail_if_not_admin2",
            "test_del_user_fail_if_not_admin2_password",
        )
        self.assertTrue(response.is_ok())
        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_del_user_fail_if_not_admin",
                "test_del_user_fail_if_not_admin_password",
            )
            self.assertTrue(response.is_ok())
            response = client.del_user("test_del_user_fail_if_not_admin2")
            self.assertTrue(response.is_denied())

    def test_del_user_fail_if_self(self):
        """
        Test that we can't delete ourselves.
        """
        response = self.client.del_user("test_del_user_fail_if_self")
        self.assertTrue(response.is_error())

    def test_check_auth_level(self):
        """
        Test that we can check the authentication level as an admin.
        """
        response = self.client.auth_level()
        self.assertEqual(response.value, AuthLevel.ADMIN)

    def test_check_username(self):
        """
        Test that we can check the username as an admin.
        """
        response = self.client.username()
        self.assertEqual(response.value, self.client.DEFAULT_ADMIN_USERNAME)


class AuthPersistenceTests(unittest.TestCase):
    """
    Test that authentication is persisted across reboots
    """

    def setUp(self) -> None:
        self.tmp_dir = TemporaryDirectory()
        self.port = get_free_port()
        self.server_handle = ServerHandle(
            ports=[self.port],
            no_auth_persist=False,
            working_dir=self.tmp_dir.name,
        )
        self.server_handle.start()

    def tearDown(self) -> None:
        self.server_handle.stop()
        self.tmp_dir.cleanup()

    def test_add_user_persist(self):
        """
        Test that we can add a user, restart the server,
        and still authenticate with the server as that user.
        """
        with make_client(self.port) as client:
            response = client.add_user(
                "test_add_user_persist", "test_add_user_persist_password"
            )
            self.assertTrue(response.is_ok())

        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_add_user_persist", "test_add_user_persist_password"
            )
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_add_user_persist", "test_add_user_persist_password"
            )
            self.assertTrue(response.is_ok())

    def test_change_password_persist(self):
        """
        Test that we can change a user's password, restart the server,
        and still authenticate with the server as that user.
        """
        with make_client(self.port) as client:
            response = client.add_user(
                "test_change_password_persist", "test_change_password_persist_password"
            )
            self.assertTrue(response.is_ok())

        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_change_password_persist", "test_change_password_persist_password"
            )
            self.assertTrue(response.is_ok())
            response = client.change_password(
                "test_change_password_persist_new_password"
            )
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_change_password_persist",
                "test_change_password_persist_new_password",
            )
            self.assertTrue(response.is_ok())

    def test_make_admin_persist(self):
        """
        Test that we can make a user an admin, restart the server,
        and still authenticate with the server as that user.
        """
        with make_client(self.port) as client:
            response = client.add_user(
                "test_make_admin_persist", "test_make_admin_persist_password"
            )
            self.assertTrue(response.is_ok())
            response = client.edit_user("test_make_admin_persist", AuthLevel.ADMIN)
            self.assertTrue(response.is_ok())

        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_make_admin_persist", "test_make_admin_persist_password"
            )
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_make_admin_persist", "test_make_admin_persist_password"
            )
            self.assertTrue(response.is_ok())
            response = client.auth_level()
            self.assertEqual(response.value, AuthLevel.ADMIN)

    def test_delete_user_persist(self):
        """
        Test that we can delete a user, restart the server,
        and not be able to authenticate with the server as that user.
        """
        with make_client(self.port) as client:
            response = client.add_user(
                "test_delete_user_persist", "test_delete_user_persist_password"
            )
            self.assertTrue(response.is_ok())

        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_delete_user_persist", "test_delete_user_persist_password"
            )
            self.assertTrue(response.is_ok())

        with make_client(self.port) as client:
            response = client.del_user("test_delete_user_persist")
            self.assertTrue(response.is_ok())

        self.server_handle.restart()

        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                "test_delete_user_persist", "test_delete_user_persist_password"
            )
            self.assertTrue(response.is_error())


class EnviromentVariablePasswordTest(unittest.TestCase):
    """
    Test setting the default password with an environment variable
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.port = get_free_port()
        cls.server_handle = ServerHandle(
            ports=[cls.port], env={"VANITY_DEFAULT_PASSWORD": "test_env_password"}
        )
        cls.server_handle.start()

    @classmethod
    def tearDownClass(cls) -> None:
        cls.server_handle.stop()

    def test_env_password(self):
        """
        Test that we can authenticate with the server with the environment variable password.
        """
        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                client.DEFAULT_ADMIN_USERNAME,
                "test_env_password",
            )
            self.assertTrue(response.is_ok())

    def test_default_password(self):
        """
        Test that we can't authenticate with the server with the default password.
        """
        with make_client(self.port, no_login=True) as client:
            response = client.auth(
                client.DEFAULT_ADMIN_USERNAME,
                client.DEFAULT_ADMIN_PASSWORD,
            )
            self.assertTrue(response.is_error())
            self.assertEqual(response.body, "incorrect password")

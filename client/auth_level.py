from enum import Enum


class AuthLevel(Enum):
    """
    Auth levels for users.
    """

    UNKNOWN = "UNKNOWN"
    USER = "USER"
    PEER = "PEER"
    ADMIN = "ADMIN"

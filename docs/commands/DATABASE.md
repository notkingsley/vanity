This document describes the basic database commands supported by a Vanity server.  
This includes commands for database management, sorted alphabetically.  

- `COPY_TO<source><dest>`  
    Copies the value at `<source>` to `<dest>` in the current database.  
    Returns `OK` if successful, or `NULL` if the source key does not exist.  
    Does nothing if the source and destination keys are the same.  

    `<source>` is the key to copy from, a `<string>`.  
    `<dest>` is the key to copy to, a `<string>`.

    Example:
    ```
    COPY_TO (4)key1 (4)key2
    OK
    ```

- `COPY_TO_DB<source><db>`  
    Copies the value at `<source>` from the current database to the database with the given index.  
    Returns `OK` if successful, or `NULL` if the source key does not exist.  
    Does nothing if the current database and the target are the same.    

    `<source>` is the key to copy from, a `<string>`.  
    `<db>` is the database to copy to, an `<int>` between 0 and 15.

    Example:
    ```
    COPY_TO_DB (4)key1 2
    OK
    ```

- `DEL<key>`  
    Deletes the key-value pair with the given key from the current database.  
    Returns `OK` if successful, or `ERROR` if the key does not exist.  
    Only authenticated clients can delete keys.  

    `<key>` is the key to delete from the database, a `<string>`.

    Example:
    ```
    DEL (4)key1
    OK
    ```

- `EXISTS<key>`  
    Returns whether the given key exists in the current database.  
    Returns `OK` if successful, or `NULL` if the key does not exist.  
    Only authenticated clients can check keys.

    `<key>` is the key to check in the database, a `<string>`.

    Example:
    ```
    EXISTS (4)key1
    OK
    ```

- `GET<key>`  
    Returns the value associated with the given key from the current database.  
    Returns `OK` with the serialized value if successful, or `NULL` if the key does not exist.  
    Only authenticated clients can get keys.  
    This works for all types of values, but it is not recommended to use this for types other than strings, integers, and floats.  

    `<key>` is the key to get from the database, a `<string>`.

    Example:
    ```
    GET (4)key1
    OK:STR (6)value1
    ```

- `KEYS`  
    Gets all keys in the current database.  
    Returns `OK` along with the serialized keys in an ARRAY if successful, or `NULL` if the database is empty.  
    Only authenticated clients can get keys.  

    Example:
    ```
    KEYS
    OK:ARR (2)[(4)key1(4)key2]
    ```

- `MANY_GET<keys>`  
    Returns the values associated with the given keys from the current database.  
    Returns `OK` along with the serialized values in a TUPLE, each with the `<type_annotation>`, adding `:NULL` for any keys that do not exist.  
    Only authenticated clients can get keys.  
    This works for all types of values, but it is not recommended to use this for types other than strings, integers, and floats.  

    `<keys>` is a list of keys to get from the database, an `<arr>` (see [ARR](../TYPES.md) documentation for request format).

    Example:
    ```
    MANY_GET (4)key1 (4)key2
    OK:TUPLE (2)(:STR (6)value1:NULL)
    ```

- `MOVE_TO<source><dest>`  
    Moves the value at `<source>` to `<dest>` in the current database.  
    Returns `OK` if successful, or `NULL` if the source key does not exist.  
    Does nothing if the source and destination keys are the same.  

    `<source>` is the key to move from, a `<string>`.  
    `<dest>` is the key to move to, a `<string>`.

    Example:
    ```
    MOVE_TO (4)key1 (4)key2
    OK
    ```

- `MOVE_TO_DB<source><db>`  
    Moves the value at `<source>` from the current database to the database with the given index.  
    Returns `OK` if successful, or `NULL` if the source key does not exist.  
    Does nothing if the current database and the target are the same.    

    `<source>` is the key to move from, a `<string>`.  
    `<db>` is the database to move to, an `<int>` between 0 and 15.

    Example:
    ```
    MOVE_TO_DB (4)key1 2
    OK
    ```

- `PERSIST`  
    Persists all databases to disk.  
    Returns `OK` if successful, or `ERROR` if persistence is disabled.  
    Only an `ADMIN` client can persist the database.  

    Example:
    ```
    PERSIST
    OK
    ```

- `RESET`  
    Resets the current database.  
    Returns `OK` if successful.  
    Only an `ADMIN` client can reset the database.  

    Example:
    ```
    RESET
    OK
    ```

- `SWITCH_DB<db_index>`  
    Switches the client to the database with the given index  
    The index must be an integer between 0 and 15.  
    Returns `OK` if successful, or `ERROR` if the index is out of range.  
    Only authenticated clients can switch databases.  

    `<db_index>` is the database to switch to, an `<int>` between 0 and 15.

    Example:
    ```
    SWITCH_DB 2
    OK
    ```

- `TYPE<key>`  
    Returns the type of the value associated with the given key from the current database.  
    Returns `OK` if successful, or `NULL` if the key does not exist.  
    Only authenticated clients can get keys.  

    `<key>` is the key to get from the database, a `<string>`.

    Example:
    ```
    TYPE (4)key1
    OK:FLOAT
    ```

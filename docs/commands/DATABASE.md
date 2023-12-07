This document describes the basic database commands supported by a Vanity server.  
This includes commands for database management, and those for primitive types, sorted alphabetically.  

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

- `FLOAT_SET<key><value>`  
    Sets the given key to the given float value in the current database, overwriting any existing value.  
    Returns `OK` if successful.  
    Only authenticated clients can set keys.  

    `<key>` is the key to set in the database, a `<string>`.  
    `<value>` is the value to set the key to, a `<float>`.  

    Example:
    ```
    FLOAT_SET (4)key1 5.5
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

- `INCR_FLOAT<key><increment>`  
    Increments the float value associated with the given key in the current database by the given increment.  
    Creates the key if it does not exist, with a value of 0.  
    Returns `OK` along with the new value if successful, or `BAD_TYPE` if the key exists and is not a float.  
    Only authenticated clients can increment float keys.  

    `<key>` is the key to increment in the database, a `<string>`.  
    `<increment>` is the amount to increment the value by, a `<float>`.  

    Example:
    ```
    INCR_FLOAT (4)key1 5.5
    OK:FLOAT 5.5
    ```

- `INCR_INT<key><increment>`  
    Increments the integer value associated with the given key in the current database by the given increment.  
    Creates the key if it does not exist, with a value of 0.  
    Returns `OK` along with the new value if successful, or `BAD_TYPE` if the key exists and is not an integer.  
    Only authenticated clients can increment integer keys.  

    `<key>` is the key to increment in the database, a `<string>`.  
    `<increment>` is the amount to increment the value by, an `<int>`.  

    Example:
    ```
    INCR_INT (4)key1 5
    OK:INT 5
    ```

- `INT_SET<key><value>`  
    Sets the given key to the given integer value in the current database, overwriting any existing value.  
    Returns `OK` if successful.  
    Only authenticated clients can set keys.  

    `<key>` is the key to set in the database, a `<string>`.  
    `<value>` is the value to set the key to, an `<int>`.  

    Example:
    ```
    INT_SET (4)key1 5
    OK
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

- `STR_LEN<key>`  
    Returns the length of the string value associated with the given key in the current database.  
    Returns `OK` along with the length if successful, or `BAD_TYPE` if the key exists and is not a string.  
    Only authenticated clients can get string lengths.  

    `<key>` is the key to get the length of in the database, a `<string>`.

    Example:
    ```
    STR_LEN (4)key1
    OK:INT 6
    ```

- `STR_SET<key><value>`  
    Sets the given key to the given string value in the current database, overwriting any existing value.  
    Returns `OK` if successful.  
    Only authenticated clients can set keys.  

    `<key>` is the key to set in the database, a `<string>`.  
    `<value>` is the value to set the key to, a `<string>`.  

    Example:
    ```
    STR_SET (4)key1 (6)value1
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

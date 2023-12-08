This document describes commands for primitive types supported by a Vanity server, sorted alphabetically.  

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

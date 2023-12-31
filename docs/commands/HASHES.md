This document describes the the HASH-related commands supported by a Vanity server  
These commands manipulate `<hash>` objects in the database, and are sorted alphabetically.  

All HASH_ commands treat a non-existent key as an empty hash, and an empty hash as a non-existent key(ie, they will be deleted from the database).  
All of these commands require basic authentication.  

- `HASH_ALL<key>`  
    Returns all fields and values of the hash stored at `<key>`.  
    Returns an empty hash if `<key>` does not exist.  
    If `<key>` exists but does not hold a hash, returns a `BAD_TYPE` error.  

    `<key>` is the key to get the hash from, a `<string>`  

    Example:
    ```
    HASH_ALL (5)mykey
    OK:HASH (2){(6)field1(6)value1(6)field2(6)value2}
    ```

- `HASH_CONTAINS<key><hash_key>`  
    Returns true if `<hash_key>` exists in the hash stored at `<key>`, false if it does not.  
    If `<key>` exists but does not hold a hash, returns a `BAD_TYPE` error.  

    `<key>` is the key to get the hash from, a `<string>`  
    `<hash_key>` is the key to check for, a `<string>`  

    Example:
    ```
    HASH_CONTAINS (5)mykey (6)field1
    OK:BOOL true
    ```

- `HASH_GET<key><hash_key>`  
    Returns the value associated with `<hash_key>` in the hash stored at `<key>`.  
    If `<key>` exists but does not hold a hash, returns a `BAD_TYPE` error.  
    If `<hash_key>` does not exist in the hash, or `<key>` does not exist returns an `ERROR(8)bad key`  

    `<key>` is the key to get the hash from, a `<string>`  
    `<hash_key>` is the key to get the value of, a `<string>`  

    Example:
    ```
    HASH_GET (5)mykey (6)field1
    OK:STR (6)value1
    ```

- `HASH_KEY_LEN<key><hash_key>`
    Returns the length of the value associated with `<hash_key>` in the hash stored at `<key>`.  
    If `<key>` exists but does not hold a hash, returns a `BAD_TYPE` error.  
    If `<hash_key>` does not exist in the hash, or `<key>` does not exist returns an `ERROR(8)bad key`  

    `<key>` is the key to get the hash from, a `<string>`  
    `<hash_key>` is the key to get the value of, a `<string>`  

    Example:
    ```
    HASH_KEY_LEN (5)mykey (6)field1
    OK:INT 6
    ```

- `HASH_KEYS<key>`  
    Returns all keys in the hash stored at `<key>`.  
    Returns an empty array if `<key>` does not exist.  
    If `<key>` exists but does not hold a hash, returns a `BAD_TYPE` error.  

    `<key>` is the key to get the hash from, a `<string>`  

    Example:
    ```
    HASH_KEYS (5)mykey
    OK:ARR (2)[(6)field1(6)field2]
    ```

- `HASH_LEN<key>`  
    Returns the number of fields contained in the hash stored at `<key>`.  
    Returns 0 if `<key>` does not exist.  
    If `<key>` exists but does not hold a hash, returns a `BAD_TYPE` error.  

    `<key>` is the key to get the hash from, a `<string>`  

    Example:
    ```
    HASH_LEN (5)mykey
    OK:INT 2
    ```

- `HASH_MANY_GET<key><hash_keys>`
    Returns the values associated with `<hash_keys>` in the hash stored at `<key>`.  
    Returns a :NULL value for every hash key that does not exist in the hash.  
    If `<key>` exists but does not hold a hash, returns a `BAD_TYPE` error.  

    `<key>` is the key to get the hash from, a `<string>`
    `<hash_keys>` is the keys to get the values of, an `<arr>` (see [ARR](../TYPES.md) documentation for request format).

    Example:
    ```
    HASH_MANY_GET (5)mykey (2)[(6)field1(6)field3]
    OK:TUPLE (2)(:STR(6)value1:NULL)
    ```

- `HASH_REMOVE<key><hash_keys>`  
    Removes the specified `<hash_keys>` from the hash stored at `<key>`.  
    Returns the number of fields that were actually removed from the hash.  
    Specified `<hash_keys>` that do not exist within this hash are ignored.  
    If `<key>` exists but does not hold a hash, returns a `BAD_TYPE` error.  
    If `<key>` does not exist, no operation is performed, and the command returns 0  

    `<key>` is the key to get the hash from, a `<string>`  
    `<hash_keys>` is the key to remove, an `<arr>` (see [ARR](../TYPES.md) documentation for request format).  

    Example:
    ```
    HASH_REMOVE (5)mykey (2)[(6)field1(6)field2]
    OK:INT 2
    ```

- `HASH_SET<key><data>`  
    Sets the value of `<key>` to `<data>`, overwriting any existing value.  
    Returns `OK` on success  

    `<key>` is the key to set the hash to, a `<string>`  
    `<data>` is the data to set the hash to, an `<hash>`  

    Example:
    ```
    HASH_SET (5)mykey (2){(6)field1(6)value1(6)field2(6)value2}
    OK
    ```

- `HASH_UPDATE<key><data>`  
    Updates the value of `<key>` to `<data>`, overwriting any existing value.  
    Returns the number of fields that were added to the hash.  

    `<key>` is the key to set the hash to, a `<string>`  
    `<data>` is the data to set the hash to, an `<hash>`  

    Example:
    ```
    HASH_UPDATE (5)mykey (2){(6)field1(6)value1(6)field2(6)value2}
    OK:INT 2
    ```

- `HASH_VALUES<key>`  
    Returns all values in the hash stored at `<key>`.
    Returns an empty array if `<key>` does not exist.  
    If `<key>` exists but does not hold a hash, returns a `BAD_TYPE` error.  

    `<key>` is the key to get the hash from, a `<string>`  

    Example:
    ```
    HASH_VALUES (5)mykey
    OK:ARR (2)[(6)value1(6)value2]
    ```

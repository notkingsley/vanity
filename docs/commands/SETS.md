This document describes the the SET-related commands supported by a Vanity server  
These commands manipulate <set> objects in the database, and are sorted alphabetically.  

All SET_ commands treat a non-existent key as an empty set, and an empty set as a non-existent key(ie, they will be deleted from the database).  
All of these commands require basic authentication.  

- `SET_ADD<key><values>`  
    Adds the elements in `<values>` to the set stored at `<key>`.  
    Values already a member of this set are ignored.  
    If key does not exist, a new set is created.  
    Returns the number of elements actually added to the set  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error  

    `<key>` - The key of the set to add to, a `<string>`  
    `<values>` - The values to add to the set a `<set>`  

    Example:
    ```
    SET_ADD (5)myset (3){(5)one(3)two(5)three}
    OK:INT 3
    ```

- `SET_ALL<key>`  
    Returns all the members of the set stored at `<key>`.  
    If `<key>` does not exist, it is treated as an empty set and this command returns an empty set.  
    If `<key>` exists and is not a set, returns a `BAD_TYPE` error  

    `<key>` - The key of the set to get the members of, a `<string>`  

    Example:
    ```
    SET_ALL (5)myset
    OK:SET (3){(5)one(3)two(5)three}
    ```

- `SET_CONTAINS<key><member>`  
    Returns true if `<member>` is a member of the set stored at `<key>`.  
    If `<key>` does not exist, it is treated as an empty set and this command returns false.  
    If `<key>` exists and is not a set, returns a `BAD_TYPE` error  

    `<key>` - The key of the set to check, a `<string>`  
    `<member>` - The member to check for, a `<string>`  

    Example:
    ```
    SET_CONTAINS (5)myset (5)one
    OK::BOOL true
    ```

- `SET_DIFF<key1><key2>`  
    Returns the difference of the set stored at `<key1>` and the set at `<key2>`.  
    If `<key1>` does not exist, it is treated as an empty set and an empty set is returned.  
    If `<key2>` does not exist, it is treated as an empty set and the set at `<key1>` is returned.  
    If any of the given keys exist and is not a set, returns a `BAD_TYPE` error  

    `<key1>` - The key of the set to diff from, a `<string>`  
    `<key2>` - The key of the set to diff with, a `<string>`   

    Example:
    ```
    SET_DIFF (5)myset (7)yourset
    OK:SET (3){(3)two(4)five(4)four}
    ```

- `SET_DIFF_INTO<dest><key1><key2>`  
    Stores the difference of the set stored at `<key1>` and the set at `<key2>` into `<dest>`.  
    If `<dest>` already exists, it is overwritten.  
    Returns the number of elements in the resulting set.  
    If `<key1>` does not exist, it is treated as an empty set and an empty set is stored in `<dest>`.  
    If `<key2>` does not exist, it is treated as an empty set and the set at `<key1>` is stored in `<dest>`.  
    If any of the given keys exist and is not a set, returns a `BAD_TYPE` error  

    `<dest>` - The key of the set to store the difference in, a `<string>`  
    `<key1>` - The key of the set to diff from, a `<string>`  
    `<key2>` - The key of the set to diff with, a `<string>`  

    Example:
    ```
    SET_DIFF_INTO (6)newset (5)myset (7)yourset
    OK:INT 3
    ```

- `SET_DIFF_LEN<key1><key2>`  
    Returns the number of elements in the difference of the set stored at `<key1>` and the set at `<key2>`.  
    If `<key1>` does not exist, it is treated as an empty set and 0 is returned.  
    If `<key2>` does not exist, it is treated as an empty set and the number of elements in the set at `<key1>` is returned.  
    If any of the given keys exist and is not a set, returns a `BAD_TYPE` error  

    `<key1>` - The key of the set to diff from, a `<string>`  
    `<key2>` - The key of the set to diff with, a `<string>`  

    Example:
    ```
    SET_DIFF_LEN (5)myset (7)yourset
    OK:INT 3
    ```

- `SET_DISCARD<key><values>`  
    Removes the elements in `<values>` from the set stored at `<key>`.  
    Non-existing elements are ignored.  
    Returns the number of elements actually removed from the set.  
    If `<key>` does not exist, it is treated as an empty set and this command returns an empty set.  
    If `<key>` exists and is not a set, returns a `BAD_TYPE` error  

    `<key>` - The key of the set to remove from, a `<string>`  
    `<values>` - The values to remove from the set a `<set>`  

    Example:
    ```
    SET_DISCARD (5)myset (3){(5)one(3)two(5)three}
    OK:INT 2
    ```

- `SET_INTERSECT<keys>`  
    Returns the intersection of all the sets given by `<keys>`.  
    If a given key does not exist, it is treated as an empty set and is ignored.  
    Returns an empty set if no keys are given.  
    If any of the given keys exist and is not a set, returns a `BAD_TYPE` error  

    `<keys>` - The keys of the sets to intersect, an `<arr>` (see [ARR](../TYPES.md) documentation for request format).  

    Example:
    ```
    SET_INTERSECT (3)[(5)myset(7)yourset(8)theirset]
    OK:SET (3){(5)one(5)three}
    ```

- `SET_INTERSECT_INTO<dest><keys>`  
    Stores the intersection of all the sets given by `<keys>` into `<dest>`.  
    If `<dest>` already exists, it is overwritten.  
    Returns the number of elements in the resulting set.  
    If a given key does not exist, it is treated as an empty set and is ignored.  
    Returns 0 if no keys are given.  
    If any of the given keys exist and is not a set, returns a `BAD_TYPE` error  

    `<dest>` - The key of the set to store the intersection in, a `<string>`  
    `<keys>` - The keys of the sets to intersect, an `<arr>` (see [ARR](../TYPES.md) documentation for request format).  

    Example:
    ```
    SET_INTERSECT_INTO (6)newset (3)[(5)myset(7)yourset(8)theirset]
    OK:INT 2
    ```

- `SET_INTERSECT_LEN<keys>`  
    Returns the number of elements in the intersection of all the sets given by `<keys>`.  
    If a given key does not exist, it is treated as an empty set and is ignored.  
    Returns 0 if no keys are given.  
    If any of the given keys exist and is not a set, returns a `BAD_TYPE` error  

    `<keys>` - The keys of the sets to intersect, an `<arr>` (see [ARR](../TYPES.md) documentation for request format).  

    Example:
    ```
    SET_INTERSECT_LEN (3)[(5)myset(7)yourset(8)theirset]
    OK:INT 2
    ```

- `SET_LEN<key>`  
    Returns the number of elements in the set stored at `<key>`.  
    If `<key>` does not exist, it is treated as an empty set and this command returns 0.  
    If `<key>` exists and is not a set, returns a `BAD_TYPE` error  

    `<key>` - The key of the set to get the length of, a `<string>`  

    Example:
    ```
    SET_LEN (5)myset
    OK:INT 3
    ```

- `SET_MOVE<source><dest><member>`  
    Moves `<member>` from the set stored at `<source>` to the set stored at `<dest>`.  
    Returns true if `<member>` is moved, false if no operation was performed.  
    If `<source>` does not exist, it is treated as an empty set and no operation is performed.  
    If `<source>` exists and is not a set, returns a `BAD_TYPE` error  
    if `<source>` does not contain `<member>`, no operation is performed.  
    If `<dest>` does not exist, a new set is created just like if `<dest>` was an empty set  
    If `<dest>` exists and is not a set, returns a `BAD_TYPE` error  
    If `<source>` becomes empty, it is deleted from the database.  

    `<source>` - The key of the set to move from, a `<string>`  
    `<dest>` - The key of the set to move to, a `<string>`  
    `<member>` - The member to move, a `<string>`  

    Example:
    ```
    SET_MOVE (5)myset (7)yourset (5)one
    OK::BOOL true
    ```

- `SET_REMOVE<key><count>`  
    Removes `<count>` random elements from the set stored at `<key>`.  
    Returns the elements removed, or an empty set if `<key>` does not exist.  
    If `<count>` is greater than the number of elements in the set, all elements are removed.  
    If `<key>` does not exist, it is treated as an empty set and this command returns an empty set.  
    If `<key>` exists and is not a set, returns a `BAD_TYPE` error  

    `<key>` - The key of the set to remove from, a `<string>`  
    `<count>` - The number of elements to remove, an `<int>`  

    Example:
    ```
    SET_REMOVE (5)myset 2
    OK:SET (2){(5)one(5)three}
    ```

- `SET_UNION<keys>`  
    Returns the union of all the sets given by `<keys>`.  
    If a given key does not exist, it is treated as an empty set and is ignored.  
    Returns an empty set if no keys are given.  
    If any of the given keys exist and is not a set, returns a `BAD_TYPE` error  

    `<keys>` - The keys of the sets to union, an `<arr>` (see [ARR](../TYPES.md) documentation for request format).  

    Example:
    ```
    SET_UNION (3)[(5)myset(7)yourset(8)theirset]
    OK:SET (5){(3)one(3)two(5)three(4)four(4)five}
    ```

- `SET_UNION_INTO<dest><keys>`  
    Stores the union of all the sets given by `<keys>` into `<dest>`.  
    If `<dest>` already exists, it is overwritten.  
    Returns the number of elements in the resulting set.  
    If a given key does not exist, it is treated as an empty set and is ignored.  
    Returns 0 if no keys are given.  
    If any of the given keys exist and is not a set, returns a `BAD_TYPE` error  

    `<dest>` - The key of the set to store the union in, a `<string>`  
    `<keys>` - The keys of the sets to union, an `<arr>` (see [ARR](../TYPES.md) documentation for request format).  

    Example:
    ```
    SET_UNION_INTO (6)newset (3)[(5)myset(7)yourset(8)theirset]
    OK:INT 5
    ```

- `SET_UNION_LEN<keys>`  
    Returns the number of elements in the union of all the sets given by `<keys>`.  
    If a given key does not exist, it is treated as an empty set and is ignored.  
    Returns 0 if no keys are given.  
    If any of the given keys exist and is not a set, returns a `BAD_TYPE` error  

    `<keys>` - The keys of the sets to union, an `<arr>` (see [ARR](../TYPES.md) documentation for request format).

    Example:
    ```
    SET_UNION_LEN (3)[(5)myset(7)yourset(8)theirset]
    OK:INT 5
    ```

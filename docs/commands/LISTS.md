This document describes the the LIST-related commands supported by a Vanity server  
These commands manipulate `<list>` objects in the database, and are sorted alphabetically.  

All LIST_ commands treat a non-existent key as an empty list, and an empty list as a non-existent key(ie, they will be deleted from the database).  
All LIST_ commands treat negative indices as offsets from the end of the list. For example, -1 is the last element of the list, -2 is the second last, and so on.  
All of these commands require basic authentication.  

- `LIST_GET<key><index>`  
    Returns the value at `<index>` in the list stored at `<key>`  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error  
    If `<index>` is out of range or `<key>` does not exist, returns an `ERROR(19)index out of range`  

    `<key>` is the key to be queried, a `<string>`  
    `<index>` is the index to be queried, an `<int>`    

    Example:
    ```
    LIST_GET (6)mylist (4)0
    OK:STR (5)hello
    ```

- `LIST_LEN<key>`  
    Returns the length of the list stored at `<key>`  
    If `<key>` does not exist, returns 0  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error  

    `<key>` is the key to be queried, a `<string>`  

    Example:
    ```
    LIST_LEN (6)mylist
    OK:INT 0
    ```

- `LIST_POP_LEFT<key><count>`  
    Pops `<count>` elements from the left of the list stored at `<key>`  
    Returns the popped elements  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error  
    If `<count>` is positive and out of range, pops to the end of the list  
    If `<count>` is negative and out of range, returns an empty list  

    `<key>` is the key to be popped from, a `<string>`  
    `<count>` is the number of elements to be popped, an `<int>`  

    Example:
    ```
    LIST_POP_LEFT (6)mylist 1
    OK:LIST (1)[(5)hello]
    ```

- `LIST_POP_RIGHT<key><count>`  
    Pops `<count>` elements from the right of the list stored at `<key>`  
    Returns the popped elements  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error  
    If `<count>` is positive and out of range, pops to the end of the list  
    If `<count>` is negative and out of range, returns an empty list  

    `<key>` is the key to be popped from, a `<string>`  
    `<count>` is the number of elements to be popped, an `<int>`  

    Example:
    ```
    LIST_POP_RIGHT (6)mylist 1
    OK:LIST (1)[(5)world]
    ```

- `LIST_PUSH_LEFT<key><data>`  
    Pushes `<data>` to the left of the list stored at `<key>`  
    Returns the new length of the list  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error  

    `<key>` is the key to be pushed to, a `<string>`  
    `<data>` is the data to be pushed, a `<list>`  

    Example:
    ```
    LIST_PUSH_LEFT (6)mylist (2)[(5)hello(5)world]
    OK:INT 2
    ```

- `LIST_PUSH_RIGHT<key><data>`  
    Pushes `<data>` to the right of the list stored at `<key>`  
    Returns the new length of the list  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error  

    `<key>` is the key to be pushed to, a `<string>`  
    `<data>` is the data to be pushed, a `<list>`  

    Example:
    ```
    LIST_PUSH_RIGHT (6)mylist (2)[(5)hello(5)world]
    OK:INT 2
    ```

- `LIST_RANGE<key><start><end>`  
    Returns the elements from `<start>` to `<end>` in the list stored at `<key>` (inclusive)  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error  
    Returns an empty list if the range is out of bounds or invalid  

    `<key>` is the key to be queried, a `<string>`  
    `<start>` is the start index, an `<int>`  
    `<end>` is the end index, an `<int>`  

    Example:
    ```
    LIST_RANGE (6)mylist 0 1
    OK:LIST (2)[(5)hello(5)world]
    ```

- `LIST_REMOVE<key><element><count>`  
    Removes `<count>` instances of `<element>` from the list stored at `<key>`  
    Removes from the left if `<count>` is positive, and from the right if `<count>` is negative  
    Removes all instances if `<count>` is 0  
    Returns the number of removed elements  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error    

    `<key>` is the key to be trimmed, a `<string>`  
    `<element>` is the element to be removed, a `<string>`  
    `<count>` is the number of elements to be removed, an `<int>`  

    Example:
    ```
    LIST_REMOVE (6)mylist (5)hello 1
    OK:INT 1
    ```

- `LIST_SET<key><index><data>`  
    Sets the value at `<index>` in the list stored at `<key>` to `<data>`  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error  
    If `<index>` is out of range or `<key>` does not exist, returns an `ERROR(19)index out of range`  

    `<key>` is the key to be queried, a `<string>`  
    `<index>` is the index to be queried, an `<int>`  
    `<data>` is the data to be set, a `<string>`  

    Example:
    ```
    LIST_SET (6)mylist (4)0 (5)hello
    OK:STR (5)hello
    ```

- `LIST_TRIM<key><start><end>`  
    Trims the list stored at `<key>` to the elements from `<start>` to `<end>` (inclusive)  
    Returns the number of trimmed elements  
    If `<key>` exists and is not a list, returns a `BAD_TYPE` error  

    `<key>` is the key to be trimmed, a `<string>`  
    `<start>` is the start index, an `<int>`  
    `<end>` is the end index, an `<int>`  

    Example:
    ```
    LIST_TRIM (6)mylist 0 1
    OK:INT 0
    ```

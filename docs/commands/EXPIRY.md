This document the expiry commands and mechanisms of a Vanity server.
All expiry commands require basic authentication.

- `CLEAR_EXPIRY<key>`  
    Clears the expiry of the key `<key>`.  
    Always returns `OK`, even if the key does not exist.  

    `<key>` is the key to clear the expiry of, a `<string>`  

    Example:
    ```
    CLEAR_EXPIRY (5)mykey
    OK
    ```

- `GET_EXPIRY<key>`  
    Gets the expiry of the key `<key>`.  
    Returns `OK` with the expiry in seconds if the key exists and has an expiry.  
    Returns `NULL` if the key does not exist or does not have an expiry.  

    `<key>` is the key to get the expiry of, a `<string>`

    Example:
    ```
    GET_EXPIRY (5)mykey
    OK:FLOAT 10.12345
    ```

- `SET_EXPIRY<key><seconds>`  
    Sets the expiry of the key `<key>` to `<seconds>` seconds from now.  
    Always returns `OK`, even if the key does not exist.  

    `<key>` is the key to set the expiry of, a `<string>`  
    `<seconds>` is the number of seconds from now to expire the key, a `<float>`  

    Example:
    ```
    SET_EXPIRY (5)mykey 10.5
    OK
    ```

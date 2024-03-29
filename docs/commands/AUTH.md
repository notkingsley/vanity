This document describes the AUTH-related commands and responses.

## Auth commands
- `ADD_USER<username><password>`  
    Adds a new user to the server.  
    An `OK` response indicates success, an `ERROR` response indicates failure, with the `<body>` containing the reason.  
    A `DENIED` response is returned if the requesting client is not an `ADMIN`.  

    `<username>` is the username to authenticate with, a `<string>`, minimum 4 characters,  
    `<password>` is the password to authenticate with, a `<string>`, minimum 4 characters.  

    Example:
    ```
    ADD_USER (5)user1 (5)password1
    OK
    ```

- `AUTH<username><password>`  
    Authenticates with the server.  
    An `OK` response indicates success, an `ERROR` response indicates failure, with the <body> containing the reason.  
    This can only be done once per connection. Subsequent attempts will receive a `DENIED` response.  

    `<username>` is the username to authenticate with, a `<string>`,  
    `<password>` is the password to authenticate with, a `<string>`.  

    Example:
    ```
    AUTH (5)user1 (9)password1
    OK
    ```

- `CHANGE_PASSWORD<new_password>`  
    Changes the password of the currently authenticated user.  
    An `OK` response indicates success, an `ERROR` response indicates failure, with the `<body>` containing the reason.  

    Only authenticated users can change their password.  
    The password cannot be changed for another user.  

    `<new_password>` is the new password to set, a `<string>`, minimum 4 characters.  

    Example:
    ```
    CHANGE_PASSWORD (5)pass1
    OK
    ```

- `DEL_USER<username>`
    Deletes a user from the server.
    An `OK` response indicates success, an `ERROR` response indicates failure, with the `<body>` containing the reason.  
    A DENIED response is returned if the requesting client is not an `ADMIN`.  

    Note that a user cannot delete themself.  
    If any client is currently connected with the user being deleted, they will be disconnected on the next request.  

    `<username>` is the username to authenticate with, a `<string>`

    Example:
    ```
    DEL_USER (5)user1
    OK
    ```

- `EDIT_USER<username><auth_level>`  
    Edits a user's authentication level.  
    An `OK` response indicates success, an `ERROR` response indicates failure, with the `<body>` containing the reason.  
    A `DENIED` response is returned if the requesting client is not an `ADMIN`.  

    `<username>` is the username to authenticate with, a `<string>`
    `<auth_level>` is the authentication level to set, see the AUTH_LEVELS documentation for more information.

    Only the following auth levels can be set:
    - `ADMIN`
    - `USER`

    Example:
    ```
    EDIT_USER (5)user1 ADMIN
    OK
    ```


- `AUTH_LEVEL`  
    Returns the authentication level of the currently authenticated user.  
    An `OK` response is returned, with the `<value>` containing the authentication level, an `AUTH_LEVEL` object.  

    Example:
    ```
    AUTH_LEVEL
    OK:AUTH_LEVEL ADMIN
    ```


- `USERNAME`  
    Returns the username of the currently authenticated user.  
    An `OK` response is returned, with the `<value>` containing the username, a `<string>`.  

    Example:
    ```
    USERNAME
    OK:STR (5)user1
    ```

### AUTH_LEVELS
`<auth_level>` is one of the following:
- `ADMIN`
    An administrator. Can perform all actions except `AUTH`.
- `USER`
    A regular user. Can perform most actions, except:
    - `ADD_USER`
    - `DEL_USER`
    - `EDIT_USER`
    - `RESET`
    - `TERMINATE`
- `UNKNOWN`
    An unauthenticated client, the default state on connection. Can only perform the following actions:
    - `AUTH`
    - `PING`
    - `EXIT`
    - `AUTH_LEVEL`
- `PEER`
    A peer client. Not currently used.
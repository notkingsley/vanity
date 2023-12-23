This document describes the basic commands supported by a Vanity Server.  
Other documents describe the commands for some specific purposes.

- `TERMINATE`
    Terminates the server.
    This can only be done by an `ADMIN` client.
    Does not return a response.

- `EXIT`
    Terminates the current client's connection.
    Does not return a response.

- `PING<message>`
    Returns a `PONG` response, and a `<body>` which is the same as the `<message>` in the request.

- `PIPE<pipe>`
    A PIPE request, containing multiple requests in one command.  
    The `<pipe>` is a sequence of requests, as defined in the [TYPES](../TYPES.md) documentation.  
    `PIPE` requests get an `:AGG` response, which is a sequence of responses, one for each request in the pipe, in order.
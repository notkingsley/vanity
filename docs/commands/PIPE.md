This document describes the behaviour of pipe requests

A pipe is simply a mechanism to execute multiple commands in a single request.  
A pipe request begins with the `PIPE` command, followed by a sequence of requests simply concatenated together.  

A pipe request is defined as follows:
- `PIPE<pipe>`

- `<pipe>` => `<length><pipe_body>`  
    where  
    - `<length>` is a length as defined in [TYPES](../TYPES.md),
    - `<pipe_body>` is the pipe itself. Exactly len characters are read after the opening closing parentheses and no escape sequences are recognized.

- `<pipe_body>` => `<request>...`  
    where  
    - `<request>` is a request as defined in the [SYNTAX documentation](../SYNTAX.md),

    No extra characters are expected to demarcate the sequence of requests.

The server will execute each request in the sequence, and return an `:AGG` response, one response in the aggregate for each request in the sequence, in the same order.  

All auth and accounts related commands are not allowed in a pipe request.  
Any other request can be included in a pipe request, including another pipe request, or even an entire transaction.  
Each request in the sequence is executed in the same context as if it were executed individually.

A malformed request in a pipe request will poison the rest of the sequence, and each subsequent request will receive a [`BAD_REQUEST` response](../STATUS_CODES.md).
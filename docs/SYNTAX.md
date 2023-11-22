This document describes the syntax used by Vanity over socket connections.

The connections are fundamentally message-oriented, so the protocol is effectively a Request-Response cycle.
It is guaranteed that there will be exactly one response for each request, with the exception of the `TERMINATE` and `EXIT` commands.  
If multiple requests are sent without waiting for a response, it is guaranteed that distinct responses will be received for each, but it is undefined the order in which the responses will be received.


In most cases, whitespace is ignored, but note:
- The potential whitespace in between a string body, which is preserved.
- Whitespace after a `<length>` field is not ignored (see [TYPES](./TYPES.md) documentation).


Over the wire, the syntax is defined as follows:
- A `<message>` is defined as a sequence of bytes, with the following structure:  
`<message>` => `<message_length><request>  `  
where:  
  - `<message_length>` is a 4-byte unsigned integer, in network byte order, representing the length of the message in bytes.  
  - `<request>` is a sequence of bytes of the length specified in the length field.

- A `<request>` is defined as a sequence of characters, with the following structure:  
`<request>` => `<command><arguments>  `  
where:
    - `<command>` is one of the commands described in the [commands/](./commands/) documentations.
    - `<arguments>` is a sequence of `<object>`s, as described in the [TYPES](./TYPES.md) documentation

- A `<response>` is defined as a sequence of characters, with one of the following structure:  
`<response>` => `<status>` or `<status><body>` or `<status><response_value>`  
where:
    - `<status>` is one of the status codes described in the [STATUS CODES](./STATUS_CODES.md) documentation.
    - `<body>` is a `<string>`, as described in the [TYPES](./TYPES.md) documentation.
    - `<response_value>` is the value of the response, an `<object>` as described in the [TYPES](./TYPES.md) documentation.

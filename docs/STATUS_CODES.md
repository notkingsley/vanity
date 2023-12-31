This document describes the status codes used by Vanity in responses to requests.

A `<status>` is defined as a simple sequence of characters and is one of the following:
- `OK` - The request was successful.
- `ERROR` - An error occurred while processing the request.
- `NULL` - Some requested data was not found.
- `PONG` - A ping request was successful.
- `DENIED` - The request was denied due to insufficient permissions.
- `BAD_REQUEST` - The request was malformed.
- `BAD_TYPE` - A command was issued on a key of the wrong type.
- `INTERNAL_ERROR` - An internal error occurred while processing the request.
- `BAD_STATE` - The client is in an incorrect state to make a request with that command.
- `QUEUED` - The request was queued for later execution.
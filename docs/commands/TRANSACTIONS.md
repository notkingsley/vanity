This document describes transactions as supported by Vanity.

A transaction can only contain database commands, and is executed as a single atomic operation with respect to other requests on the same database.  
Most commands that are not database commands are not allowed in a transaction.  
Commands that switch databases, or use multiple databases, are not allowed in a transaction.  
Since only database commands are allowed, a pipe request is not allowed in a transaction, although a pipe request can contain transactions.  

Once a transaction is started, only commands that dirrectly access the database are allowed, or one of the following commands:
- `PING`
- `EXIT`
- `TERMINATE`
- `TRANSACT_COMMIT`
- `TRANSACT_DISCARD`
- `PUBLISH`
- `SUBSCRIBE`
- `UNSUBSCRIBE`

Vanity transactions are supported with the following commands:

- `TRANSACT_BEGIN`  
  Starts a transaction, putting the current client in transaction mode.  
  This command is only allowed if the client is already in transaction mode.  
  Always returns an `OK` response.

  All database requests after this command, until a `TRANSACT_COMMIT` or `TRANSACT_DISCARD` command, are part of the transaction.  
  Allowed database commands receive a `QUEUED` response, and are queued for execution when the transaction is committed.  
  Other allowed commands are executed as normal.  
  Commands not allowed receive a `BAD_STATE` response.  
  Malformed requests are not queued, and receive a `BAD_REQUEST` response immediately.  

  Permissions are checked when the transaction is committed, not when the command is received, so it is possible to get a `DENIED` response when the transaction is committed, even if the command was allowed when it was received.


- `TRANSACT_COMMIT`
  Commits the current transaction, executing all queued commands, and putting the client back in normal mode.    
  This command is only allowed if the client is in transaction mode, it will receive a `BAD_STATE` response otherwise.  
  Returns an `:AGG` response, containing a response for each command in the transaction, in the same order.


- `TRANSACT_DISCARD`
  Discards the current transaction, discarding all queued commands, and putting the client back in normal mode.  
    This command is only allowed if the client is in transaction mode, it will receive a `BAD_STATE` response otherwise.
    Returns an `OK` response.
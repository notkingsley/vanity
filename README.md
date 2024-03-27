# Vanity

**Vanity** is an in-memory distributed database/ data structure server.  
At the moment, the "distributed" part is still WIP, but everything else works just fine.

The server side implementation is C++,  
The test client (and unit tests) are in Python and I might create distributable client packages in the future, starting with Python.


### Features
**Vanity** supports:
-  ```string```, ```int``` , ```float```, ```list```, ```set```, and ```hash``` data types with several [commands](docs/commands/).  
-  Secure authentication with Admin and User roles, achieved with bcrypt hashing.
-  Automatic persistence to disk.
-  Key expiries.
-  ACID operations implemented with a Write-Ahead-Log.
-  Crash recovery and guarantees.
-  Sixteen independent databases (configurable at compile time).
-  Pipelining of commands.
-  Pub/Sub messaging.
-  Transactions, also ACID compliant, and can be pipelined.

There's a lot more to come, but this is a good start.  
[*Welcome to the Playground. Follow me...*](https://youtu.be/3jfI-z__GY0?t=30)


## Targets
See [features](#features) above. 
Major stuff coming up:

- Distribution/Replication (weeeeeeeeee):  
  The server will be able to run on multiple nodes, with data distributed across them.  
  This will be achieved with a consistent hashing algorithm, and a gossip protocol for node discovery and failure detection.  
  The system will be able to handle node failures and rebalance data accordingly.  
  The system will also be able to replicate data across nodes for fault tolerance.  
  The system will be able to handle network partitions and merge data when the partition heals.  
  The system will be able to handle node joins and leaves without downtime.  
  The system will be able to handle data rebalancing without downtime

- More data types (maybe):  
  ```bitmap```, ```hyperloglog```, ```geospatial```, and ```streams```.

## Syntax
**Vanity** implements its own [protocol](docs/SYNTAX.md) over sockets. It was not designed to be human-readable, but it's not too bad.  
A client would abstracts all this away anyway.  

Nothwithstanding, the communication is still mostly [message-oriented](docs/SYNTAX.md): a request-response cycle.  
There are ASYNC responses, sent unsolicited when a request was not made. Currently, this only delivers pubsub messages.


The [client](client/client_.py) handles the protocol, so you can do:
```python
>>> from client import Client
>>> client = Client("localhost", 9955)
>>> response = client.str_set("foo", "bar")
>>> response.is_ok()
True
>>> response = client.get("foo")
>>> response.value
'bar'
>>> client.exit()
```

The underlying syntax, used over the sockets and expected by the included [vanity_cli](client/vanity_cli.py), is much less appealing (see the [docs](docs/SYNTAX.md) for more details):
```bash
$ ./client/vanity_cli.py localhost 9955
> PING
PONG
> STR_SET (3)foo (3)bar
OK
> GET (3)foo
OK:STR (3)bar
> EXIT
```

**Vanity** is *NOT* production ready, (and is indeed not intended for such purposes), see [Redis](https://redis.io) instead.  

The similarities with Redis are, in fact, coincidental.  
I came up with something entirely different, then after a few million years of evolution, it ended up looking like Redis, so I figured I'd just damce along.

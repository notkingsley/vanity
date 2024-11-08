# Vanity

**Vanity** is an in-memory distributed database/ data structure server.  
Vanity is not framework-dependent and has no build-time, link-time, or runtime dependencies.

The server side implementation is pure modern C++ (C++20).  
The test client (and unit tests) are in Python and there might be distributable client packages in the future, starting with Python.  

There's also an intention to support Vanity as an embedded database, with official bindings for Python, C++, and maybe Rust.


### Features
**Vanity** provides:
-  Secure authentication with Admin and User roles, achieved with bcrypt hashing.
-  Continuous persistence to disk, including WAL guarantees.
-  Automatic object expiries.
-  ACID transactions implemented (in revision to use MVCC).
-  Crash recovery and guarantees.
-  Sixteen independent databases (configurable at compile time).
-  Pipelining of commands for efficient querying.
-  Pub/Sub messaging.
-  ```string```, ```int``` , ```float```, ```list```, ```set```, and ```hash``` data types with several [commands](docs/commands/).  

**Vanity** is designed to be fast, with a focus on low latency and high throughput.  
There's a lot more to come, [*welcome to the Playground, follow me...*](https://youtu.be/3jfI-z__GY0?t=30)


## In Progress
-  **Replication with [Raft](https://raft.github.io/)**:  
  Raft is the consensus algorithm used by etcd, Apache Kafka, RabbitMQ, and many others.  
  The target is an asynchronous, single-leader system with a guarantee of eventual consistency.

-  **Multiversion Concurrency Control ([MVCC](https://en.wikipedia.org/wiki/Multiversion_concurrency_control))**:  
  Also used by relational databases like Postgres, Vanity will use MVCC for serializable isolation.  
  This bit has been, to be honest, very difficult. 

## Targets
Major stuff coming up:

- Stable Client APIs:  
  Official client libraries for Python, C++ (and maybe Rust). The Python client already exists and is used for testing.

- An embeddable Vanity:  
  A standalone distribution of libvanity, with official bindings for Python.

- More data types (maybe):  
  ```bitmap```, ```hyperloglog```, ```geospatial```, and ```streams```.

- Partitioning (maybe, haha):  
  Sharding, likely with hash-based key partitioning and fixed-size partitions.


## Performance
**Vanity** was designed to be fast, with a focus on low latency and high throughput.  
With response times as low as hundredths of a millisecond, (~40 microseconds), it definitely delivers on that promise.

Redis is a great piece of software, and I'm not trying to compete with it, but here I make some comparisons.  
These tests are made on the same machine repeatedly under the same conditions, and so are only useful when comparing with each other and are NOT absolute or indicative.  

###### Latency
Redis ping latency is as slow as 2.25 milliseconds, Vanity's slowest is only 0.212 milliseconds.  
The average ping latency is only ~45% better than Redis (0.184ms vs 0,103ms), but the worst case is over 10 times better.

###### Throughput
For simple Get/Set requests, with a total of 200,000 operations and a concurrency of 100, Vanity maintains an average response time less than 4.5ms while Redis takes 5.7ms  
With two million operations and a thousand concurrent clients, the average response climbs to 54ms for Vanity and 70ms for Redis.  

Thus, in the average case, Vanity is about 23% faster than Redis, and in the worst case, it's over 60% faster.

Redis doesn't allow large values (over 512MB), but Vanity doesn't have such a limit.  
Redis, however, does outperform Vanity at that limit by almost half.


## Building
**Vanity** is built with CMake, and the build process is very simple.  
There are no link-time or runtime dependencies, and the only build time dependency is a C++20 compiler.

See [BUILD.md](BUILD.md) for more details.


## Syntax
**Vanity** implements its own [protocol](docs/SYNTAX.md) over sockets.  
It was designed for efficient single-pass parsing and not to be human-readable, but it's obvious to follow, if quite a pain to type manually.  
A client would abstracts all this away anyway.  

The communication remains [message-oriented](docs/SYNTAX.md): a request-response cycle.  
There are ASYNC responses, sent unsolicited when a request was not made.  
Currently, these only delivers pubsub messages (and internally, replication data between peers). In the future, they may include server notifications.


The [client](client/client_.py) handles the protocol, so you can do:
```python
>>> from client import Client
>>> client = Client("localhost", 9955)
>>> client.str_set("foo", "bar")
<Response: status=OK>
>>> response = client.get("foo")
>>> response.value
'bar'
>>> client.exit()
```

The underlying syntax, used over the sockets and expected by the included [vanity_cli](client/vanity_cli.py), is less appealing (see the [docs](docs/SYNTAX.md) for more details):
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

**Vanity** may be production-ready, but it is not battle-tested ⚔️.  
If you would like to test it, please do so and report any issues you find.  

**PS:** Any similarities with Redis are coincidental. I wanted to build a database, and I came up with something that eventually evolved into Vanity.

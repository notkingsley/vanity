# Vanity

**Vanity** intends to be an in-memory distributed data structure server.

The server side implementation is to be fully C++,  
The test client and the tests themselves are in Python and I intend to create distributable client packages for other languages (including C++) in the future, starting with Python.


Data types currently supported are ```string```, ```int``` , ```float```, ```list```, ```set```, and ```hash```.  

Obviously, we've a long way to go. A very long way, really.  
*Welcome to the Playground. Follow me...*


## Targets
Here are the major features I plan to implement, in order (hopefully):

- Persistence *- implemented* 
- Permissions *- implemented*
- Sessions (auth etc) *- implemented*
- Key expiry (TTL)
- Atomicity (or transactional behaviour, of sorts)
- Read/write guarantees (to some yet unspecified degree)
- Distribution/Clustering (this is, by far, the fun part, and the reason I'm doing this at all)

I imagine the biggest challenge by far will come in ensuring consistency of transactions (or locks) across the distributed system. *How is locking to be implemented over a distributed system* for example.  
In truth, I do not know, but I'm REALLY looking forward to finding out.


Of course, all that doesn't include actually implementing the data structures themselves, which is a whole other challenge.  
But I'm here for the fun, so ***Let Fall the Iron Rain***


### Syntax
**Vanity** bypasses all communication protocols and uses sockets for communication. The client abstracts this away  
The overhead of HTTP is unnecessary here, so that's what we have. 
A lightweight WebSocket implementation would also serve here, but it's still not really needed. 

Nothwithstanding, the primitive communication is still message-oriented. Something of a request-response like HTTP, but reusing the same socket connection. This might change still, so I'm retaining the event-oriented advantage (potentially).  


The [client](client/client_.py) abstracts most of it, so you can do:
```python
>>> from client import Client
>>> client = Client("localhost", 9955)
>>> response = client.set("foo", "bar")
>>> response.is_ok()
True
>>> response = client.get("foo")
>>> response.value
'bar'
>>> client.exit()
```

The underlying syntax (used over the sockets and expected by the [cli](client/vanity_cli.py)) is less appealing (see the [docs](docs/SYNTAX.md) for more details)
```bash
$ ./client/vanity_cli.py localhost 9955
> PING
PONG
> SET:STR (3)foo (3)bar
OK
> GET (3)foo
OK:STR (3)bar
> EXIT
```

This is work in progress, so I'd expect a lot of changes to come.  
**Vanity** is *NOT* production ready, see [Redis](https://redis.io) instead.
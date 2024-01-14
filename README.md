# Vanity

**Vanity** is an in-memory distributed data structure server.  
At the moment, the "distributed" part is still WIP, but everything else works just fine.

The server side implementation is C++,  
The test client (and the unit tests themselves) are in Python and I might create distributable client packages for other languages (including C++) in the future, starting with Python.


**Vanity** supports ```string```, ```int``` , ```float```, ```list```, ```set```, and ```hash``` data types with several [commands](docs/commands/).  

As things stand, there's still a long way to go. A very long way, really, if you consider the distributed part.  
[*Welcome to the Playground. Follow me...*](https://youtu.be/3jfI-z__GY0?t=30)


## Targets
Here are the major features I plan to implement, in order (hopefully):

- Persistence *- implemented* 
- Permissions *- implemented*
- Sessions (auth etc) *- implemented*
- Key expiry (TTL) *- implemented*
- Atomicity (or transactional behaviour, of sorts) *- implemented*
- Pub/Sub *- implemented*
- Read/write guarantees (to some yet unspecified degree)
- Distribution/Clustering (this is, by far, the fun part, and the reason I'm doing this at all)


I imagine the biggest challenge by far will come in ensuring consistency of transactions (or locks) across the distributed system(or not...).  
*How is locking to be implemented over a distributed system* for example.  
In truth, I do not know ([yet](https://en.wikipedia.org/wiki/Distributed_lock_manager)), but I'm REALLY looking forward to finding out.

*Cackles delighfully, "[***Let Fall the Iron Rain!!***](https://red-rising.fandom.com/wiki/Iron_Rain)"*


Of course, all that doesn't include several other features I'd be adding along the way, which I won't bother you with.  
But I'm here for the [Thrill](https://coppermind.net/wiki/Nergaoul#The_Thrill), so sing along.


## Syntax
**Vanity** implements its own [protocol](docs/SYNTAX.md) over sockets. It was not designed to be human-readable, but it's not too bad.  
The client abstracts all this away anyway.  

Nothwithstanding, the communication is still mostly [message-oriented](docs/SYNTAX.md), a request-response cycle.  
There are ASYNC responses, sent even when a request was not made. Currently, this only delivers publised messages.


The [client](client/client_.py) abstracts most of it, so you can do:
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
> PING (11)hello world
PONG:STR (11)hello world
> STR_SET (3)foo (3)bar
OK
> GET (3)foo
OK:STR (3)bar
> EXIT
```

This is work in progress, so I'd expect a lot of changes to come.  
**Vanity** is *NOT* production ready, (and is indeed not intended for such purposes), see [Redis](https://redis.io) instead.  

The similarities with Redis are, in fact, coincidental: I came up with something entirely different, then after a few million years of evolution, it ended up looking like Redis, so I figured I'd just model it after Redis.
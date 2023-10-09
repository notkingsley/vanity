# Vanity

**Vanity** intends to be an in-memory distributed data structure server.

The server side implementation is to be fully C++,  
The test client and the tests themselves are in Python and I intend to create distributable client packages for other languages (including C++) in the future, starting with Python.


Data types currently supported are ```string```, ```int``` and  ```float```.  
More will come later.

Obviously, we've a long way to go. A very long way, really.  
*Welcome to the Playground. Follow me...*


## Targets
Here are the major features I plan to implement, in order (hopefully):

- Persistence *- implemented* 
- Permissions *- implemented*
- Sessions (handshaking phase on connection to allow selecting db index to use and other things)
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
A lightweight WebSocket implementation would also serve here, and I might even consider it in the future. 

Nothwithstanding, the primitive communication is still message-oriented. Something of a request-response like HTTP, but reusing the same socket connection. This might change still, so I'm retaining the event-oriented advantage (potentially).  


The current underlying syntax (used over the sockets and expected by the cli) looks like:  
> GET "foo"  
>> NULL

> SET :STR "foo" "bar"
>> OK

>GET "foo"
>> :STR "bar"

> DEL "double"
>> ERROR

> SET :FLOAT "double" 123.456
>> OK

> DEL "foo"
>> OK

> GET "double"
>> :FLOAT 123.456

> EXIT


The client abstracts this away, so you can just do:
```python
from client import Client
client = Client("localhost", 9955)
client.set("foo", "bar")
print(client.get("foo"))
client.exit()
```

This is work in progress, so I'd expect a lot of changes to come.  
**Vanity** is *NOT* production ready, see [Redis](https://redis.io) instead.
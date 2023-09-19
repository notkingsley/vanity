# Vanity

**Vanity** intends to be an in-memory distributed data structure server.

The server side implementation is to be fully C++, but the test clients are in Python and I intend to create distributable packages for other languages in the future, starting with Python.

The only 'data structure' currently supported is a string, so it is, at present, a key-value store at best.

Obviously, we've a long way to go. A very long way, really.  
Follow me...


## Targets
Here are the main features I plan to implement, in order (hopefully):

- Persistence
- Atomicity (or transactional behaviour, of sorts)
- Read/write guarantees (to some unspecified degree)
- Distribution/Clustering (this is the fun part)

I imagine the biggest challenge by far will come in ensuring consistency of transactions (or locks) across the distributed system. *How is locking to be implemented over a distributed system* for example.  
In truth, I do not know, but I'm **really** looking forward to finding out.

I am also considering auth/security at some point, and the design already allows for this, but really, I don't know yet.

Of course, all that doesn't include actually implementing the data structures themselves, which is a whole other challenge.  
But I'm here for the fun, so ***Let Fall the Iron Rain***
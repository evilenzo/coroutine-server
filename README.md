# coroutine-server
> Boost::Beast echo server example written with C++20 standard stackless coroutines
``` commandline
$ coroutine-server <address> <port> <threads>
```
### Build requirements
- Boost::system
- fmt (presented by a submodule)
### Example
``` commandline
$ coroutine-server 127.0.0.1 8080 4 &
$ curl -X GET 127.0.0.1:8080 -d "Hello, World!"
Hello, World!
```

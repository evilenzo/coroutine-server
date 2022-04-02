# coroutine-server
> Boost.Beast HTTP echo server example based on C++20 stackless coroutines
``` console
$ coroutine-server <address> <port> <threads>
```
### Build requirements
- Boost::system
- fmt (presented by a submodule)
### Example
```console
$ coroutine-server 127.0.0.1 8080 4 &
$ curl -X GET 127.0.0.1:8080 -d "Hello, World!"
Hello, World!
```

# coroutine-server
> Boost::Beast small echo server written with C++20 stackless coroutines
``` console
# coroutine-server <address> <port> <threads>
```
### Example
``` console
# coroutine-server 127.0.0.1 80 1 &
$ curl -X GET 127.0.0.1 -d "hello"
hello
```

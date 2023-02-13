* ```int socket(int domain, int type, int protocol)```
  - ```socket``` is used to creat a socket descriptor, which uniquely identifies a socket. 
  - domain
    - AF_INET
    - AF_INET6
    - AF_LOCAL
    - AF_ROUTE
  - type
    - SOCK_STREAM
      - using TCP
    - SOCK_DGRAM
      - using UDP
  - protocol
    - IPPROTO_TCP
    - IPPROTO_UDP
    - IPPROTO_SCTP
    - IPPROTO_TIPC

* ```int bind(int sockfd, sockaddr* addr, socklen_t addrlen)```
  - sockfd 
	- addr
	- addrlen: the length of addr

* Network byte order & Host byte order
	- Large-end storage: The low byte is placed at the high address end of memory, and the high byte is placed at the low one.
	- Small-end storage: The low byte is placed at the low address end of memory, and the high byte is placed at the high one.

  * ```uint32_t htonl(uint32_t)```
    - (host to network) Convert the unsigned long value of the host to network byte order (32-bit)

  * ```uint32_t ntohl(uint32_t)```

  * ```uint32_t ntohs(uint32_t)```

* ```int listen(int sockfd, int backlog)```
  - ```listen``` turns the socket into a passive type and wait for the client to connect.
  - backlog: the maximum connections that the corresponding socket can queue.


* ```int connect(int sockfd, sockaddr* addr, socklen_t addrlen)```
  - The client establishes a connection with the TCP server by calling the ```connect``` function.

* ```int accept(int sockfd, sockaddr* addr, socklen_t* addrlen)```
  - ```accept``` return the connected socket descriptor.

* ```read()```, ```close()```
  
* ```int close(int fd)```
  - ```close``` counts the reference count of the corresponding socket descriptor $-1$. When the reference count is $0$, the TCP client will be triggered to send a connection termination request to the server. 

* three-times handshake connetion of TCP 
  - The client sends a ```SYN 1``` to the server
  - The server sends a ```SYN 2``` to the client and confirms ```SYN``` with ```ACK 1+1```
  - The server confirms ```ACK 2+1``` to the server 

* four times waves of TCP
  - The client calls close to actively close the connection and sends a ```FIN 3``` to the server
  - After the server accepts the request, it performs a passive shutdown and confirms the ```FIN``` with ```ACK 3+1```. It is also passed to the application as a file terminator.
  - After a period of time, the application that receives the file terminator calls close to close its scoket. At the same time, the server also sends a ```FIN N```
  - Confirm the server after receiving the ```FIN```. Confirm the server.

* ```int setsockopt(int sockfd, int level, int optname, void* optval, socklen_t optlen)```
  -  ```setsockopt``` is used to set the option value of any type and any state socket interface.
  - level: the level of option definition, and it support SOL_SOCKET, IPPROTO_TCP, IPPROTO_IP, IPPROTO_IPV6
  - optname
    - SO_REUSEADDR: allow port to be reused
  - optval: pointer to the buffer of storing the new value of the option to be set.

* ```recv()```, ```send()```

# Epoll
* ```int epoll_create(int size)```
* ```int epoll_ctl(int epfd, int op, int fd, epoll_event* event)```
* ```int epoll_wait(int epfd, epoll_event* events, int amxevents, int timeout)```
* ```struct epoll_event { uint32_t events; epoll_data_t data; }```
  - events: events registered by epoll, such as 
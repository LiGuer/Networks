# Computer Networks

## Networks Architecture
* OSI (7 Layers)
  * Application Layer
  * Presentation Layer
  * Session Layer
  * Transport Layer
  * Network Layer
  * Data-link Layer
  * Physical layer

* TCP/IP (4 Layers)
  * Application Layer
  * Transport Layer
  * Network Layer
  * Local Access Layer

* Comprehensive
  * Application Layer
  * Transport Layer
  * Network Layer
  * Data-link Layer
  * Physical layer

## Physical Layer
* Equipment -- Hub: Just broadcast electrical signals to all exports.

## Data-link Layer
* MTU
* MAC Address: (Each device has unique and unchanging MAC Address)

* Equipment —— Switch: Only the computer pointed to by the target MAC address.

## Network Layer
* Equipment —— Router: As an independent device with MAC address, it can help to forward data packets once (each port of the router has an independent MAC address)

  * 子网划分、子网掩码

### Protocol
  * **IP (Internet Protocol)**

  * **ICMP (Internet Control Message Protocol)**

  * **ARP (Address Resolution Protocol)**

  * **RARP (Reverse Address Resolution Protocol)**


## Transport Layer

  * **UDP (User Datagram Protocol)**
  * UDP datagram header
    |Bit|意义|
    |---|---|
    |0-15|Source port|
    |16-31|Destination port|
    |32-47|Length|
    |48-63|Checksum|
    |||

  * Application —— 使用UDP的协议
    * DNS
    * TFTP
    * SNMP
    * NFS

  * **TCP (Transmission Control Protocol)**
  * TCP segment header
    |Bit|意义|
    |---|---|
    |0-15|Source port|
    |16-31|Destination port|
    |32-63|Sequence number|
    |64-95|Acknowledgment number (if ACK set)|
    |96-99|Data offset|
    |100-102|Reserved 000|
    |103|NS|
    |104|CWR|
    |105|ECE|
    |106|URG|
    |107|ACK|
    |108|PSH|
    |109|RST|
    |110|ACK|
    |111|FIN|
    |112-127|Window Size|
    |128-143|Checksum|
    |144-160|Urgent pointer (if URG set)|
    |||


  * 连接
    * 连接建立 —— Three-way Handshake

  * 流量控制 —— Sliding Window Protocol  
    - 滑动窗口，接收端所能提供的缓冲区大小, 发送方根据这个数据来计算自己最多能发送多长的数据. 
    - 流量控制，主要是接收方传递信息给发送方，使其不要发送数据太快，是一种端到端的控制。主要的方式就是返回的ACK中会包含自己的接收窗口的大小，并且利用大小来控制发送方的数据发送

  * 拥塞控制  
    拥塞窗口 cwnd (congestion window)  
    当cwnd < ssthresh时，使用慢开始算法。  
    当cwnd > ssthresh时，改用拥塞避免算法。  
    当cwnd = ssthresh时，慢开始与拥塞避免算法任意。  

    * 慢开始  
      不要一开始就发送大量的数据，先探测一下网络的拥塞程度，也就是说由小到大逐渐增加拥塞窗口的大小。

    * 拥塞避免  
      - 加法增大
      - 乘法减小

    * 快重传, 快恢复  
  
  * Application —— 使用TCP的协议
    * FTP
    * Telnet
    * SMTP
    * POP3
    * HTTP

  * 存在问题
    * SYN泛洪攻击  

    * TCP粘包  

    * TCP心跳包

## Application Layer
* 数据传输基本单位为报文

### Protocol
  * **FTP (File Transfer Protocol)**

  * **Telnet**

  * **DNS (Domain Name System)**

  * **SMTP (Simple Mail Transfer Protocol)**

  * **POP3 (Post Office Protocol - Version 3)**

  * **HTTP (Hyper Text Transfer Protocol)**

    * Format
      - GET：请求读取由URL所标志的信息
      - POST：给服务器添加信息（如注释）
      - PUT：在给定的URL下存储一个文档
      - DELETE：删除给定的URL所标志的资源

    * HTTP 状态码  

      |Code|Description|  
      |---|---|
      |1**|信息, 服务器收到请求, 需要请求者继续执行操作|
      |100|Continue
      |101|Switching Protocols
      |||
      |2**|成功, 操作被成功接收并处理
      |200|OK|
      |201|Created|
      |202|Accepted|
      |203|Non-Authoritative Information|
      |204|No Content|
      |205|Reset Content|
      |205|Reset Content|
      |||
      |3**|重定向,需进一步操作以完成请求|
      |301|Moved Permanently|
      |||
      |4**|客户端错误,请求包含语法错误或无法完成请求|
      |400|Bad Request|
      |401|Unauthorized|
      |403|Forbidden|
      |404|Not Found|
      |404|Method Not Allowed  |
      |||
      |5**|服务器错误, 服务器在处理请求的过程中发生了错误|
      |500|Internal Server Error|
      |||


  * **HTTP (Hyper Text Transfer Protocol over Secure Socket Layer)**

  * **Cookie / Session**

## 一个实例

# linux内核中网络相关知识积累

### 1.内核中将ip转换成string
ip地址一般是一个Unit_32的整数，在内核中以ip(xx.xx.xx.xx)的形式输出一个ip
```

loginfo("In match_tcp_redirect every math rule: %u.%u.%u.%u -> %u.%u.%u.%u\n",
               HIPQUAD(unit32_t ip), HIPQUAD(unit32_t ip));
```

有时候函数是需要一个ip的字符串，比如说in_aton，将ip转成string
```
sprintf(stringIpBuf, "%d.%d.%d.%d",HIPQUAD(unit32_t ip));
```

### 2.触发网络操作
```
tcp连接测试:
1.curl -v http://1.1.1.1:80
2.wget -O - http://9.9.9.6
udp连接测试：(两边互相能接收到消息)
1. nc -l -u -v 55554:监听本地的55554端口
2. nc -u -v 127.0.0.1 55555：往本地的55555端口发送消息。

```

### 3.使用tcpdump抓包
```
1.ip -4 a
1.tcpdump -i enp0s3 -n tcp -w /tmp/tcp.pcap:把tcp的包抓到/tmp/tcp.pcap目录下。
```

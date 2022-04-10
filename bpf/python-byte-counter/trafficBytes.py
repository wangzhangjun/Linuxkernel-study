from bcc import BPF
import time
from socket import inet_ntop, AF_INET, AF_INET6
from collections import namedtuple, defaultdict
from struct import pack


def tcptrace_compile():
    b = BPF(src_file="trafficBytes.c")
    ipv4_send_bytes = b["ipv4_send_bytes"]
    ipv4_recv_bytes = b["ipv4_recv_bytes"]
    ipv6_send_bytes = b["ipv6_send_bytes"]
    ipv6_recv_bytes = b["ipv6_recv_bytes"]
    return (ipv4_send_bytes, ipv4_recv_bytes, ipv6_send_bytes, ipv6_recv_bytes)


def tcptrace_func(ipv4_send_bytes, ipv4_recv_bytes, ipv6_send_bytes, ipv6_recv_bytes):
    TCPSessionKey = namedtuple('TCPSession', ['pid', 'laddr', 'lport', 'daddr', 'dport'])
    def pid_to_comm(pid):
        try:
            comm = open("/proc/%d/comm" % pid, "r").read().rstrip()
            return comm
        except IOError:
            return str(pid)

    # ????ipv4
    def get_ipv4_session_key(k):
        return TCPSessionKey(pid=k.pid,
                            laddr=inet_ntop(AF_INET, pack("I", k.saddr)),
                            lport=k.lport,
                            daddr=inet_ntop(AF_INET, pack("I", k.daddr)),
                            dport=k.dport)

    # ????ipv6
    def get_ipv6_session_key(k):
        return TCPSessionKey(pid=k.pid,
                            laddr=inet_ntop(AF_INET6, k.saddr),
                            lport=k.lport,
                            daddr=inet_ntop(AF_INET6, k.daddr),
                            dport=k.dport)
    # IPv4: build dict of all seen keys
    ipv4_throughput = defaultdict(lambda: [0, 0])
    # ??????
    for k, v in ipv4_send_bytes.items():
        key = get_ipv4_session_key(k)
        ipv4_throughput[key][0] = v.value
    ipv4_send_bytes.clear()
    # ??????
    for k, v in ipv4_recv_bytes.items():
        key = get_ipv4_session_key(k)
        ipv4_throughput[key][1] = v.value
    ipv4_recv_bytes.clear()

    # ????ipv4????
    ipv4_data = [
        {
            "time": time.time(),
            "pid": int(k.pid),
            "com": pid_to_comm(k.pid),
            "saddr": k.laddr,
            "sport": str(k.lport),
            "daddr": k.daddr,
            "dport": str(k.dport),
            "recv_byte": int(recv_bytes),
            "send_byte": int(send_bytes)
        }
        for k, (send_bytes, recv_bytes) in ipv4_throughput.items()
    ]

    # IPv6: build dict of all seen keys
    ipv6_throughput = defaultdict(lambda: [0, 0])
    # ??????
    for k, v in ipv6_send_bytes.items():
        key = get_ipv6_session_key(k)
        ipv6_throughput[key][0] = v.value
    ipv6_send_bytes.clear()
    # ??????
    for k, v in ipv6_recv_bytes.items():
        key = get_ipv6_session_key(k)
        ipv6_throughput[key][1] = v.value
    ipv6_recv_bytes.clear()

    # ????ipv6????
    ipv6_data = [
        {
            "time": time.time(),
            "pid": int(k.pid),
            "com": pid_to_comm(k.pid),
            "saddr": k.laddr,
            "sport": str(k.lport),
            "daddr": k.daddr,
            "dport": str(k.dport),
            "recv_byte": int(recv_bytes),
            "send_byte": int(send_bytes)
        }
        for k, (send_bytes, recv_bytes) in ipv6_throughput.items()
    ]
    return (ipv4_data, ipv6_data)

if __name__ == '__main__':
    ip4s, ip4r, ip6s, ip6r = tcptrace_compile()
    while True:
        try:
            tcp4_datas, tcp6_datas = tcptrace_func(ip4s, ip4r, ip6s, ip6r)
            for i in tcp4_datas:
                print(i)
            for i in tcp6_datas:
                print(i)
            time.sleep(1)
        except KeyboardInterrupt:
            break
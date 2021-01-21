#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE); // create netlink socket
    if (fd < 0)
    {
        printf("Failed to create netlink socket: %s\n", (char *)strerror(errno));
        return 1;
    }
    struct sockaddr_nl local; // local addr struct
    char buf[8192];           // message buffer
    //只是分配一个iovec，而不是数组，每次只收一个
    struct iovec iov;          // message structure
    iov.iov_base = buf;        // set message buffer as io
    iov.iov_len = sizeof(buf); // set size

    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK; // set protocol family
    local.nl_pid = getpid();      // set out id using current process id
    // RTMGRP_LINK (network interface create/delete/up/down events)
    //RTMGRP_IPV4_IFADDR (IPv4 addresses add/delete events) multicast groups.
    local.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE;
    //为recvmsg做准备
    struct msghdr msg;
    msg.msg_name = &local;           // local address
    msg.msg_namelen = sizeof(local); // address size
    msg.msg_iov = &iov;              // io vector
    msg.msg_iovlen = 1;              // io size
    if (bind(fd, (struct sockaddr *)&local, sizeof(local)) < 0)
    { // bind socket
        printf("Failed to bind netlink socket: %s\n", (char *)strerror(errno));
        close(fd);
        return -1;
    }
    while (1)
    {
        //MSG_DONTWAIT 设置为非阻塞模式
        //阻塞I/O : socket 的阻塞模式意味着必须要做完IO 操作（包括错误）才会返回。
        //非阻塞I/O : 非阻塞模式下无论操作是否完成都会立刻返回，需要通过其他方式来判断具体操作是否成功
        ssize_t recvlen = recvmsg(fd, &msg, MSG_DONTWAIT); //返回读到的字节数
        if (recvlen < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
            {
                usleep(250000);
                continue;
            }

            printf("Failed to read netlink: %s", (char *)strerror(errno));
            continue;
        }
        if (msg.msg_namelen != sizeof(local))
        { // check message length, just in case
            printf("Invalid length of the sender address struct\n");
            continue;
        }
        // message parser
        /*
		netlink消息包头：
		struct nlmsghdr
		{
		__u32 nlmsg_len;   //包括报头在内的所有消息长度
		__u16 nlmsg_type;  //消息类型，四种(NLMSG_LOOP,NLMSG_ERROR,NLMSG_NONE, NLMSG_OVERRUN(这里有RTM_NEWLINK等，可以参考rtnetlink.h))
		__u16 nlmsg_flags; //参考uapi/linux/netlink.h
		__u32 nlmsg_seq;   //序列号，但是netlink并未要求必须使用序列号
		__u32 nlmsg_pid;   //发送端的pid
		};
	    */
        struct nlmsghdr *h;
        //拿出一个消息，使用NLMSG_OK判断是否OK，NLMSG_OK本质是判断h消息是否比recvlen大，如果不大的话说明出错了，否则正确处理，处理完后尽心下一个NLMSG_NEXT
        for (h = (struct nlmsghdr *)buf; NLMSG_OK(h, recvlen); h = NLMSG_NEXT(h, recvlen))
        {
            switch (h->nlmsg_type)
            {
            case NLMSG_DONE:
                return 0;
            case NLMSG_ERROR:
                break;
            case RTM_NEWROUTE:
            case RTM_DELROUTE:
                break;
            case RTM_NEWADDR:
            case RTM_DELADDR:
                break;
            case RTM_NEWLINK:
            case RTM_DELLINK:
                break;
            default:
                break;
            }
        }
    }
    return 0;
}

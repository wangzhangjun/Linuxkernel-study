#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <sys/types.h>
#include <unistd.h>

void parse_rtattr(struct rtattr **tb, int max, struct rtattr *attr, int len)
{
    for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
    {
        if (attr->rta_type <= max)
        {
            tb[attr->rta_type] = attr;
        }
    }
}
void nl_netroute_handle(struct nlmsghdr *nlh)
{
    /*
    触发：
    netstat -rn查看gateway
    给main表添加一条gateway ：ip route add 192.168.2.0/24 via 10.211.55.1 table main
    */
    int len;
    struct rtattr *tb[RTA_MAX + 1];
    struct rtmsg *rt;
    char tmp[256];
    bzero(tb, sizeof(tb));
    rt = NLMSG_DATA(nlh);
    len = nlh->nlmsg_len - NLMSG_SPACE(sizeof(*rt));
    parse_rtattr(tb, RTA_MAX, RTM_RTA(rt), len);
    printf("%s: ", (nlh->nlmsg_type == RTM_NEWROUTE) ? "NEWROUT" : "DELROUT");
    if (tb[RTA_DST] != NULL)
    {
        inet_ntop(rt->rtm_family, RTA_DATA(tb[RTA_DST]), tmp, sizeof(tmp));
        printf("DST: %s ", tmp);
    }
    if (tb[RTA_SRC] != NULL)
    {
        inet_ntop(rt->rtm_family, RTA_DATA(tb[RTA_SRC]), tmp, sizeof(tmp));
        printf("SRC: %s ", tmp);
    }
    if (tb[RTA_GATEWAY] != NULL)
    {
        inet_ntop(rt->rtm_family, RTA_DATA(tb[RTA_GATEWAY]), tmp, sizeof(tmp));
        printf("GATEWAY: %s ", tmp);
    }
    printf("\n");
}
void nl_netifaddr_handle(struct nlmsghdr *nlh)
{
    //RTM_NEWADDR, RTM_DELADDR, RTM_GETADDR
    //添加,删除或者接收一个和接口相关的IP地址的信息。
    //这些消息含有一个ifaddrmsg类型的结构，紧跟在后面的是一系列的rtattr结构。
    /*
    struct ifaddrmsg {
        unsigned char ifa_family;    
        unsigned char ifa_prefixlen; 
        unsigned char ifa_flags;     
        unsigned char ifa_scope;     
        int ifa_index;               
    };
    ifa_family: 地址类型（通常为AF_INET or AF_INET6)）
    ifa_prefixlen: 地址的地址掩码长度，如果改地址定义在这个family
    ifa_flags:
    ifa_scope: 地址的作用域
    ifa_index:  接口索引与接口地址关联

            Attributes （rtattr部分属性，rta_type）
 rta_type        value type             description
 ─────────────────────────────────────────────────────────────
 IFA_UNSPEC      -                      unspecified.
 IFA_ADDRESS     raw protocol address   接口地址 interface address
 IFA_LOCAL       raw protocol address   本地地址 local address
 IFA_LABEL       asciiz string          接口名称 name of the interface
 IFA_BROADCAST   raw protocol address   广播 broadcast address.
 IFA_ANYCAST     raw protocol address   anycast address
 IFA_CACHEINFO   struct ifa_cacheinfo   Address information.
 IFA_MULTICAST,
 __IFA_MAX,
 */

    int len;
    struct rtattr *tb[IFA_MAX + 1];
    struct ifaddrmsg *ifaddr;
    char tmp[256];
    bzero(tb, sizeof(tb));

    ifaddr = NLMSG_DATA(nlh);
    len = nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifaddr));
    parse_rtattr(tb, IFA_MAX, IFA_RTA(ifaddr), len);
    printf("%s ", (nlh->nlmsg_type == RTM_NEWADDR) ? "NEWADDR" : "DELADDR");
    if (tb[IFA_LABEL] != NULL)
    {
        printf("%s ", (char *)RTA_DATA(tb[IFA_LABEL]));
    }
    if (tb[IFA_ADDRESS] != NULL)
    {
        inet_ntop(ifaddr->ifa_family, RTA_DATA(tb[IFA_ADDRESS]), tmp, sizeof(tmp));
        printf("%s ", tmp);
    }
    printf("\n");
}
void nliterfaceinfo_handle(struct nlmsghdr *nlh)
{
    //RTM_NEWLINK, RTM_DELLINK, RTM_GETLINK
    //创建或者删除一个特定的网络接口，或者从一个特定的网络接口上获得信息。
    //这些消息含有一个ifinfomsg类型的结构，紧跟在后面的是一系列的rtattr结构。
    /* 
    struct ifinfomsg
    {
        unsigned char ifi_family; 
        unsigned short ifi_type;  
        int ifi_index;            
        unsigned int ifi_flags;   
        unsigned int ifi_change;  
    }; 
    
    ifi_family: 接口地址类型
    ifi_type: 设备类型
    ifi_index: 是结构唯一的索引
    ifi_flags:  设备标志，可以看netdevice 结构
    ifi_change: 保留值，通常设置为0xFFFFFFFF
    
ifi_type代表硬件设备的类型：
ARPHRD_ETHER                   10M以太网
ARPHRD_PPP                     PPP拨号
ARPHRDLOOPBACK                 环路设备
     
ifi_flags包含设备的一些标志：
     IFF_UP                            接口正在运行
     IFF_BROADCAST                     有效的广播地址集
     IFF_DEBUG                         内部调试标志
     IFF_LOOPBACK                      这是自环接口
     IFF_POINTOPOINT                   这是点到点的链路设备
     IFF_RUNNING                       资源已分配
     IFF_NOARP                         无arp协议，没有设置第二层目的地址
     IFF_PROMISC                       接口为杂凑(promiscuous)模式
     IFF_NOTRAILERS                    避免使用trailer
     IFF_ALLMULTI                      接收所有组播(multicast)报文
     IFF_MASTER                        主负载平衡群(bundle)
     IFF_SLAVE                         从负载平衡群(bundle)
     IFF_MULTICAST                     支持组播(multicast)
     IFF_PORTSEL                       可以通过ifmap选择介质(media)类型
     IFF_AUTOMEDIA                     自动选择介质
     IFF_DYNAMIC                       接口关闭时丢弃地址    

Routing attributes（rtattr部分属性，rta_type）
 rta_type         value type         description
  ──────────────────────────────────────────────────────────
 IFLA_UNSPEC      -                  未说明，未指定的数据
 IFLA_ADDRESS     hardware address   L2硬件地址
 IFLA_BROADCAST   hardware address   L2广播地址.
 IFLA_IFNAME      asciiz string      char型设备名.
 IFLA_MTU         unsigned int       MTU of the device.
 IFLA_LINK        int                Link type.
 IFLA_QDISC       asciiz string      Queueing discipline.
 IFLA_STATS       see below          struct rtnl_link_stats的设备信息
 IFLA_COST,
 IFLA_PRIORITY,
 IFLA_MASTER,
 IFLA_WIRELESS,
 IFLA_PROTINFO, 
 IFLA_TXQLEN,
 IFLA_MAP,
 IFLA_WEIGHT,
 IFLA_OPERSTATE,
 IFLA_LINKMODE,
 __IFLA_MAX

 struct rtattr
{
        unsigned short rta_len;
        unsigned short rta_type;  //这个type 就是上面enum 定义的类型
        //Data follows 
};

//相关的宏，用来获取ifinfomsg后面的rtattr结构
#define IFLA_RTA(r) ((struct rtattr *)(((char *)(r)) + NLMSG_ALIGN(sizeof(struct ifinfomsg))))
IFLA_RTA(r) r为ifinfomsg的基地址，返回ifinfomsg后的第一个rtattr基地址
//此宏展开为NLMSG_PAYLOAD(nlh, sizeof（ifinfomsg）)，获取PAYLOAD的长度
IFLA_PAYLOAD(n) 
// 判断是否为合法的路由属性 
#define RTA_OK(rta,len) ((len) >= (int)sizeof(struct rtattr) && \
             (rta)->rta_len >= sizeof(struct rtattr) && \
              (rta)->rta_len <= (len))
int RTA_OK(struct rtattr *rta, int rtabuflen);

//获取下一个rtattr的首地址
#define RTA_NEXT(rta,attrlen)   ((attrlen) -= RTA_ALIGN((rta)->rta_len), \
                  (struct rtattr*)(((char*)(rta)) + RTA_ALIGN((rta)->rta_len)))
struct rtattr *RTA_NEXT(struct rtattr *rta, unsigned int rtabuflen);

//返回加上 rtattr header的总长度
#define RTA_LENGTH(len) (RTA_ALIGN(sizeof(struct rtattr)) + (len))
unsigned int RTA_LENGTH(unsigned int length);

//返回数据对齐的最小值
#define RTA_SPACE(len)  RTA_ALIGN(RTA_LENGTH(len))
unsigned int RTA_SPACE(unsigned int length);

//返回属性数据部分首地址
#define RTA_DATA(rta)   ((void*)(((char*)(rta)) + RTA_LENGTH(0)))
void *RTA_DATA(struct rtattr *rta);

//返回属性数据部分的长度
#define RTA_PAYLOAD(rta) ((int)((rta)->rta_len) - RTA_LENGTH(0))
unsigned int RTA_PAYLOAD(struct rtattr *rta);
*/

    int len;
    struct rtattr *tb[IFLA_MAX + 1];
    struct ifinfomsg *ifinfo;
    bzero(tb, sizeof(tb));
    ifinfo = (struct ifinfomsg *)NLMSG_DATA(nlh);
    //宏NLMSG_SPACE(len)返回不小于NLMSG_LENGTH(len)且字节对齐的最小数值，它也用于分配消息缓存。
    //这里是否可以直接使用NLMSG_PAYLOAD(nlh, sizeof(struct ifinfomsg));
    len = nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifinfo));
    parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifinfo), len);
    printf("%s: %s ", (nlh->nlmsg_type == RTM_NEWLINK) ? "NEWLINK" : "DELLINK", (ifinfo->ifi_flags & IFF_UP) ? "up" : "down");
    if (tb[IFLA_IFNAME])
    {
        printf("%s", (char *)RTA_DATA(tb[IFLA_IFNAME])); //RTA_DATA返回属性数据部分首地址
    }
    printf("\n");
}

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
                nl_netroute_handle(h); //路由变化
                break;
            case RTM_NEWADDR:
            case RTM_DELADDR:
                nl_netifaddr_handle(h); //ip地址的改变
                break;
            case RTM_NEWLINK:
            case RTM_DELLINK:
                nliterfaceinfo_handle(h); //interface的up或者down等
                break;
            default:
                break;
            }
        }
    }
    return 0;
}

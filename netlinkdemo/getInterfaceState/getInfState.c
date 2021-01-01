#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/netdevice.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>

struct net_device_stats
{
    unsigned long rx_packets;
    unsigned long tx_packets;
    unsigned long rx_bytes;
    unsigned long tx_bytes;
    unsigned long rx_errors;
    unsigned long tx_errors;
    unsigned long rx_dropped;
    unsigned long tx_dropped;
    unsigned long multicast;
    unsigned long collisions;
    unsigned long rx_length_errors;
    unsigned long rx_over_errors;
    unsigned long rx_crc_errors;
    unsigned long rx_frame_errors;
    unsigned long rx_fifo_errors;
    unsigned long rx_missed_errors;
    unsigned long tx_aborted_errors;
    unsigned long tx_carrier_errors;
    unsigned long tx_fifo_errors;
    unsigned long tx_heartbeat_errors;
    unsigned long tx_window_errors;
    unsigned long rx_compressed;
    unsigned long tx_compressed;
};

int main()
{
    int nSocket = 0, nLen = 0, nAttrLen = 0;
    char szBuffer[8192] = {0};
    struct
    {
        struct nlmsghdr nh;
        struct ifinfomsg ifi;
    } struReq;

    struct sockaddr_nl struAddr;
    struct nlmsghdr *pstruNL;
    struct ifinfomsg *pstruIF;
    struct rtattr *pstruAttr;
    struct net_device_stats *pstruInfo;
    struct ether_addr *pstruEther;

    nSocket = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (nSocket < 0)
    {
        fprintf(stderr, "create socket error:%s\n", strerror(errno));
        return -1;
    }
    memset(&struAddr, 0, sizeof(struAddr));
    struAddr.nl_family = AF_NETLINK;
    struAddr.nl_pad = 0;
    struAddr.nl_pid = getpid();
    struAddr.nl_groups = 0;
    int retBind = bind(nSocket, (struct sockaddr *)&struAddr, sizeof(struAddr));
    if (retBind < 0)
    {
        fprintf(stderr, "bind error:%s\n", strerror(errno));
    }
    //发送请求
    memset(&struReq, 0, sizeof(struReq));
    /*构造头：nlmsghdr*/
    struReq.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struReq)); //NLMSG_LENGTH包含抱头在内的消息长度
    struReq.nh.nlmsg_type = RTM_GETLINK;
    struReq.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP; //NLM_F_REQUEST:消息为请求消息；NLM_F_DUMP：检索有关表、条目信息
    struReq.nh.nlmsg_pid = getpid();
    struReq.ifi.ifi_family = AF_UNSPEC; //AF_INET表示ipv4,AF_INET6表示ipv6,AF_UNSEPEC表示任意的

    memset(&struAddr, 0, sizeof(struAddr));
    struAddr.nl_family = AF_NETLINK;
    struAddr.nl_pad = 0;
    struAddr.nl_pid = 0;
    struAddr.nl_groups = 0;

    int retSend = sendto(nSocket, &struReq, struReq.nh.nlmsg_len, 0, (struct sockaddr *)&struAddr, sizeof(struAddr));
    if (retSend < 0)
    {
        fprintf(stderr, "sendto error:%s\n", strerror(errno));
    }

    while ((nLen = recv(nSocket, szBuffer, sizeof(szBuffer), 0)))
    {
        pstruNL = (struct nlmsghdr *)szBuffer;
        //数据有效性检测
        while (NLMSG_OK(pstruNL, nLen)) //因为nlmsghdr中存放的是整个消息的长度，以此来判断数据是否合法
        {
            if (pstruNL->nlmsg_type == NLMSG_DONE) //数据已经结束
                break;
            if (pstruNL->nlmsg_type == NLMSG_ERROR)
            {
                //出错
                struct nlmsgerr *pstruError;
                pstruError = (struct nlmsgerr *)NLMSG_DATA(pstruNL); //NLMSG_DATA 消息数据部分的首地址
                fprintf(stderr, "发生 error:%s\n", strerror(pstruError->error));
                break;
            }
            //正常消息，获取数据
            pstruIF = NLMSG_DATA(pstruNL);
            fprintf(stderr, "获取设备[%d]的信息:\n", pstruIF->ifi_index);
            fprintf(stderr, "\t设备类型:");
            switch (pstruIF->ifi_type)
            {
            case ARPHRD_ETHER:
                fprintf(stderr, "以太网\n");
                break;
            case ARPHRD_PPP:
                fprintf(stderr, "ppp拨号\n");
                break;
            case ARPHRD_LOOPBACK:
                fprintf(stderr, "回环设备\n");
                break;
            default:
                fprintf(stderr, "未知\n");
                break;
            }
            fprintf(stderr, "\t设备状态: ");
            if ((pstruIF->ifi_flags & IFF_UP) == IFF_UP)
                fprintf(stderr, " UP");
            if ((pstruIF->ifi_flags & IFF_BROADCAST) == IFF_BROADCAST)
                fprintf(stderr, " BROADCAST");
            if ((pstruIF->ifi_flags & IFF_DEBUG) == IFF_DEBUG)
                fprintf(stderr, " DEBUG");
            if ((pstruIF->ifi_flags & IFF_LOOPBACK) == IFF_LOOPBACK)
                fprintf(stderr, " LOOPBACK");
            if ((pstruIF->ifi_flags & IFF_POINTOPOINT) == IFF_POINTOPOINT)
                fprintf(stderr, " POINTOPOINT");
            if ((pstruIF->ifi_flags & IFF_RUNNING) == IFF_RUNNING)
                fprintf(stderr, " RUNNING");
            if ((pstruIF->ifi_flags & IFF_NOARP) == IFF_NOARP)
                fprintf(stderr, " NOARP");
            if ((pstruIF->ifi_flags & IFF_PROMISC) == IFF_PROMISC)
                fprintf(stderr, " PROMISC");
            if ((pstruIF->ifi_flags & IFF_NOTRAILERS) == IFF_NOTRAILERS)
                fprintf(stderr, " NOTRAILERS");
            if ((pstruIF->ifi_flags & IFF_ALLMULTI) == IFF_ALLMULTI)
                fprintf(stderr, " ALLMULTI");
            if ((pstruIF->ifi_flags & IFF_MASTER) == IFF_MASTER)
                fprintf(stderr, " MASTER");
            if ((pstruIF->ifi_flags & IFF_SLAVE) == IFF_SLAVE)
                fprintf(stderr, " SLAVE");
            if ((pstruIF->ifi_flags & IFF_MULTICAST) == IFF_MULTICAST)
                fprintf(stderr, " MULTICAST");
            if ((pstruIF->ifi_flags & IFF_PORTSEL) == IFF_PORTSEL)
                fprintf(stderr, " SLAVE");
            if ((pstruIF->ifi_flags & IFF_AUTOMEDIA) == IFF_AUTOMEDIA)
                fprintf(stderr, " AUTOMEDIA");
            if ((pstruIF->ifi_flags & IFF_DYNAMIC) == IFF_DYNAMIC)
                fprintf(stderr, " DYNAMIC");
            fprintf(stderr, "\n");
            //pstruIF是整个payload的头，整个payload是由头和其他的attr组成的，所以接下来就是计算attr
            pstruAttr = IFLA_RTA(pstruIF);    //IFLA_RTA用来获取ifinfomsg后面的rtattr结构
            nAttrLen = IFLA_PAYLOAD(pstruNL); //获取attr payload的长度
            while (RTA_OK(pstruAttr, nAttrLen))
            {
                switch (pstruAttr->rta_type)
                {
                case IFLA_IFNAME:
                    fprintf(stderr, "\t设备名称：%s\n", (char *)RTA_DATA(pstruAttr));
                    break;
                case IFLA_MTU:
                    fprintf(stderr, "\t设备mtu：%d\n", *(unsigned int *)RTA_DATA(pstruAttr));
                    break;
                case IFLA_QDISC:
                    fprintf(stderr, "\t设备队列:%s\n",
                            (char *)RTA_DATA(pstruAttr));
                    break;
                case IFLA_ADDRESS:
                    if (pstruIF->ifi_type == ARPHRD_ETHER)
                    {
                        pstruEther = (struct ether_addr *)
                            RTA_DATA(pstruAttr);
                        fprintf(stderr, "\tMAC地址:%s\n",
                                ether_ntoa(pstruEther));
                    }
                    break;
                case IFLA_BROADCAST:
                    if (pstruIF->ifi_type == ARPHRD_ETHER)
                    {
                        pstruEther = (struct ether_addr *)
                            RTA_DATA(pstruAttr);
                        fprintf(stderr, "\t广播MAC地址:%s\n",
                                ether_ntoa(pstruEther));
                    }
                    break;
                case IFLA_STATS:
                    pstruInfo = (struct net_device_stats *)RTA_DATA(pstruAttr);
                    fprintf(stderr, "\t接收信息:\n");
                    fprintf(stderr, "\t\t接收报文:%lu 字节:%lu\n",
                            pstruInfo->rx_packets, pstruInfo->rx_bytes);
                    fprintf(stderr, "\t\terrors:%lu dropped:%lu "
                                    "multicast:%lu collisions:%lu\n",
                            pstruInfo->rx_errors, pstruInfo->rx_dropped,
                            pstruInfo->multicast, pstruInfo->collisions);
                    fprintf(stderr, "\t\tlength:%lu over:%lu crc:%lu "
                                    "frame:%lu fifo:%lu missed:%lu\n",
                            pstruInfo->rx_length_errors,
                            pstruInfo->rx_over_errors,
                            pstruInfo->rx_crc_errors,
                            pstruInfo->rx_frame_errors,
                            pstruInfo->rx_fifo_errors,
                            pstruInfo->rx_missed_errors);
                    fprintf(stderr, "\t发送信息:\n");
                    fprintf(stderr, "\t\t发送报文:%lu 字节:%lu\n",
                            pstruInfo->tx_packets, pstruInfo->tx_bytes);
                    fprintf(stderr, "\t\terrors:%lu dropped:%lu\n",
                            pstruInfo->tx_errors, pstruInfo->tx_dropped);
                    fprintf(stderr, "\t\taborted:%lu carrier:%lu fifo:%lu"
                                    " heartbeat:%lu window:%lu\n",
                            pstruInfo->tx_aborted_errors,
                            pstruInfo->tx_carrier_errors,
                            pstruInfo->tx_fifo_errors,
                            pstruInfo->tx_heartbeat_errors,
                            pstruInfo->tx_window_errors);
                    break;
                default:
                    break;
                }
                pstruAttr = RTA_NEXT(pstruAttr, nAttrLen);
            }
            pstruNL = NLMSG_NEXT(pstruNL, nLen);
        }
    }
    return 0;
}
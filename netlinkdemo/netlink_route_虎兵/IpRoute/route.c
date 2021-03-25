#include <stdio.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <sys/types.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <unistd.h>
#include <string.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>


typedef __u32 u32;


int addattr_l(struct nlmsghdr *n, int maxlen, int type, void *data, int alen)
{
	int len = RTA_LENGTH(alen);
    struct rtattr *rta;
    if (NLMSG_ALIGN(n->nlmsg_len) + len > maxlen)
        return -1;
    rta = (struct rtattr*)(((char*)n) + NLMSG_ALIGN(n->nlmsg_len));
    rta->rta_type = type;
    rta->rta_len = len;
    memcpy(RTA_DATA(rta), data, alen);
    n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + len;
    fprintf(stderr,"\nattr len=%d\n",n->nlmsg_len);
    return 0;
}


void route_add(int fd, __u32* destination,__u32* gateway,unsigned int netmask)
{
	/*
		netlink套接字的地址：
		nl_family:AF_NETLINK
		nl_p:0
		struct sockaddr_nl {
			sa_family_t nl_family;  //总是为AF_NETLINK
			unsigned short nl_pad;  //总是为0
			pid_t nl_pid;           //单播地址，对于内核netlink，该值为0， 用户空间将其设置为进程的pid
			u_32 nl_groups;         //组播组
		}nl;
	*/
    struct sockaddr_nl nladdr; 
    int status;
	
    __u32 index=2; /* Output Interface ::: eth1，因为我的虚拟机没有eth0，是eth1接口所以是2，lo接口1，不知道其他机器是否有出入*/
    //__u32 source=254;
    // structure of the netlink packet.
	struct
    {
        struct nlmsghdr n;
        struct rtmsg r;
        char buf[1024];
    } req;
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

    status=bind(fd,(struct sockaddr *)&nladdr,sizeof(nladdr));
    if(status<0){
        perror("bind");
        exit(1);
    }
	// Forming the iovector with the netlink packet.
	//    struct iovec iov = { (void*)&req.n, req.n.nlmsg_len };

	// Forming the message to be sent.
	//    struct msghdr msg = { (void*)&nladdr, sizeof(nladdr), &iov, 1, NULL, 0, 0 };

    memset(&req, 0, sizeof(req));

	// Initialisation of a few parameters
    memset(&nladdr,0,sizeof(nladdr));
    nladdr.nl_family= AF_NETLINK;
    nladdr.nl_pid=0;
    nladdr.nl_groups=0;

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); 
    req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_EXCL;
    req.n.nlmsg_type = RTM_NEWROUTE;

    req.r.rtm_family = AF_INET;
    req.r.rtm_table = RT_TABLE_MAIN;
    req.r.rtm_protocol = RTPROT_STATIC;
    req.r.rtm_scope = RT_SCOPE_UNIVERSE;
    req.r.rtm_type = RTN_UNICAST;
    req.r.rtm_dst_len=netmask;   //目的网络子网掩码位数
    req.r.rtm_src_len=0;
    req.r.rtm_tos=0;
    req.r.rtm_flags=RT_TABLE_MAIN;

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)) ;

// RTA_DST and RTA_GW are the two esential parameters for adding a route,
// there are other parameters too which are not discussed here. For ipv4,
// the length of the address is 4 bytes.
    addattr_l(&req.n, sizeof(req), RTA_DST, destination, 4);
    addattr_l(&req.n, sizeof(req), RTA_GATEWAY, gateway, 4);
    addattr_l(&req.n, sizeof(req), RTA_OIF,&index, 4);


// sending the packet to the kernel.
    status=send(fd,&req,req.n.nlmsg_len,0);
    fprintf(stderr,"\nstatus=%d,添加路由成功\n",status);

}

int main(int argc ,char ** argv)
{
    int fd;
    __u32 dest,gate;  //dest 代表目的网络或主机，gate代表网关;
    if(argc!=4){
        printf("usage:%s 10.168.0.0 24 10.168.3.1\n",argv[0]);
        exit(1);
    }
    if(atoi(argv[2])<0||atoi(argv[2])>32){
        printf("unvalible netmask bits!!must be 0-32\n");
        exit(1);
    }

    if((fd=socket(AF_NETLINK,SOCK_RAW,NETLINK_ROUTE))<0){
        printf("error creating a socket\n");
        exit(1);
    }

    dest=inet_addr(argv[1]); /* unsigned int value for dest Ip address  ，这里的目的网络或主机地址都要转换成网络字节顺序*/
    gate=inet_addr(argv[3]); /* unsigned int value for gateway address ，也要转换成网络字节顺序 */
    route_add(fd,&dest,&gate,(unsigned int)atoi(argv[2]));
    return 0;
}


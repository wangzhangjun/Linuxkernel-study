/*
 *  Display all routes
 */
#include <stdio.h>            //printf, perror
#include <string.h>           //memset, strlen
#include <stdlib.h>           //exit
#include <unistd.h>           //close
#include <sys/socket.h>       //msghdr
#include <arpa/inet.h>        //inet_ntop
#include <linux/netlink.h>    //sockaddr_nl,NLMSG_DATA
#include <linux/rtnetlink.h>  //rtgenmsg,ifinfomsg

#define BUFSIZE 8192

struct nl_req_s {
  struct nlmsghdr hdr;
  struct rtgenmsg gen;
};

void die(char *s)
{
    perror(s);
    exit(1);
}

void rtnl_print_route(struct nlmsghdr * h)
{
    struct rtmsg * rte = NLMSG_DATA(h);
    struct rtattr * attr = RTM_RTA(rte);
    int len = RTM_PAYLOAD(h);
    char dest[32] = {0};
    char gway[32] = {"unspecified"};

    if (rte->rtm_table != RT_TABLE_MAIN) {
        return;
    }

    for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
    {
        switch (attr->rta_type)
        {
        case RTA_DST:
            inet_ntop(AF_INET, RTA_DATA(attr), dest, sizeof(dest));
            break;
        case RTA_GATEWAY:
            inet_ntop(AF_INET, RTA_DATA(attr), gway, sizeof(gway));
            break;
        default:
            break;
        }
    }

    printf("%s/%d gateway %s\n", dest, rte->rtm_dst_len, gway);
}

int main(void)
{
    struct sockaddr_nl kernel;
    int s, end=0, len;
    struct msghdr msg;
    struct nl_req_s req;
    struct iovec io;
    char buf[BUFSIZE];

    //build kernel netlink address
    memset(&kernel, 0, sizeof(kernel));
    kernel.nl_family = AF_NETLINK;
    kernel.nl_groups = 0;

    //create a Netlink socket
    if ((s=socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0)
    {
        die("socket");
    }

    //build netlink message
    memset(&req, 0, sizeof(req));
    req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
    req.hdr.nlmsg_type = RTM_GETROUTE;
    req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.hdr.nlmsg_seq = 1;
    req.hdr.nlmsg_pid = getpid();
    req.gen.rtgen_family = AF_INET;

    memset(&io, 0, sizeof(io));
    io.iov_base = &req;
    io.iov_len = req.hdr.nlmsg_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_name = &kernel;
    msg.msg_namelen = sizeof(kernel);

    //send the message
    if (sendmsg(s, &msg, 0) < 0)
    {
        die("sendmsg");
    }

    // parse reply
    while (!end)
    {
        memset(buf, 0, BUFSIZE);
        msg.msg_iov->iov_base = buf;
        msg.msg_iov->iov_len = BUFSIZE;

        if ((len=recvmsg(s, &msg, 0)) < 0)
        {
            die("recvmsg");
        }

        for (struct nlmsghdr * msg_ptr = (struct nlmsghdr *)buf;
             NLMSG_OK(msg_ptr, len); msg_ptr = NLMSG_NEXT(msg_ptr, len))
        {
            switch (msg_ptr->nlmsg_type)
            {
            case NLMSG_DONE:
                end++;
                break;
            case RTM_NEWROUTE:
                rtnl_print_route(msg_ptr);
                break;
            default:
                printf("Ignored msg: type=%d, len=%d\n", msg_ptr->nlmsg_type, msg_ptr->nlmsg_len);
                break;
            }
        }
    }

    close(s);
    return 0;
}

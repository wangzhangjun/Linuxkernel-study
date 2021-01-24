#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>

#define NEXT_CMD_ARG()   \
    do                   \
    {                    \
        argv++;          \
        if (--argc <= 0) \
            exit(-1);    \
    } while (0)

/* Helper structure for ip address data and attributes */
typedef struct {
    char family;
    char bitlen;
    unsigned char data[sizeof(struct in6_addr)];
}_inet_addr;

/* Open netlink socket */
int open_netlink()
{
    struct sockaddr_nl saddr;
    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0)
    {
        perror("Failed to open netlink socket");
        return -1;
    }
    memset(&saddr, 0, sizeof(saddr));
    return sock;
}

/* Simple parser of the string IP address*/
int read_addr(char *addr, _inet_addr *res)
{
    if (strchr(addr, ':')) {
        res->family = AF_INET6;
        res->bitlen = 128;
    } else {
        res->family = AF_INET;
        res->bitlen = 32;
    }

    return inet_pton(res->family, addr, res->data);
}

int main(int argc, char **argv)
{
    int nl_cmd = -1;
    int nl_flags = -1;
    int if_idx = -1;
    int default_gw = 0;
    _inet_addr to_addr = { 0 };
    _inet_addr gw_addr = { 0 };
    while (argc > 0)
    {
        if (strcmp(*argv, "add") == 0)
        {
            nl_cmd = RTM_NEWROUTE;
            nl_flags = NLM_F_CREATE | NLM_F_EXCL;  //NLM_F_CREATE(如果不存在，创建条目)；NLM_F_EXCL(保留既有的不动)
        }else if(strcmp(*argv, "del")){
            nl_cmd = RTM_DELROUTE;
            nl_flags = 0;
        }else if (strcmp(*argv, "to") == 0) {
            NEXT_CMD_ARG(); /* skip "to" and jump to the actual destination addr */
            if (read_addr(*argv, &to_addr) != 1) {
                fprintf(stderr, "Failed to parse destination network %s\n", *argv);
                exit(-1);
            }
        }else if (strcmp(*argv, "dev") == 0) {
            NEXT_CMD_ARG(); /* skip "dev" */
            if_idx = if_nametoindex(*argv);
        }else if(strcmp(*argv, "via") == 0) {
            NEXT_CMD_ARG(); /* skip "via"*/
            /* Instead of gw address user can set here keyword "default" */
            /* Try to read this keyword and jump to the actual gateway addr */
            if (strcmp(*argv, "default") == 0) {
                default_gw = 1;
                NEXT_CMD_ARG();
            }
            if (read_addr(*argv, &gw_addr) != 1) {
                fprintf(stderr, "Failed to parse gateway address %s\n", *argv);
                exit(-1);
            }
        }
        argc--; argv++;
    }

    int nl_sock = open_netlink();
    if (nl_sock < 0)
    {
        printf("open netlink error\n");
        exit(-1);
    }
    return 0;
}

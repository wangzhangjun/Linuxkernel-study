#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>

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

int main(int argc, char **argv)
{
    int nl_sock = open_netlink();
    if (nl_sock < 0)
    {
        printf("open netlink error\n");
        exit(-1);
    }
    return 0;
}
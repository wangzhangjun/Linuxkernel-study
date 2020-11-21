#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>

#define NETLINK_TEST 25

typedef struct Security_module_options {
    int fs_options;
    int net_options;
    int usb_options;
    int port_options;
}SecurityModuleOptions;

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;
struct sockaddr_nl src_addr, dest_addr;

SecurityModuleOptions security_module_options;

int main(int args, char *argv[]) {
    
    security_module_options.fs_options = 0;
    security_module_options.net_options = 1;
    security_module_options.usb_options = 0;
    security_module_options.port_options = 1;

    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    memset(&msg, 0, sizeof(msg));
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */
    src_addr.nl_groups = 0; /* not in mcast groups */
    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(SecurityModuleOptions)));
    /* Fill the netlink message header */
    nlh->nlmsg_len = NLMSG_SPACE(sizeof(SecurityModuleOptions));
    nlh->nlmsg_pid = getpid(); /* self pid */
    nlh->nlmsg_flags = 0;
    memcpy(NLMSG_DATA(nlh), &security_module_options, sizeof(SecurityModuleOptions));

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    printf(" Sending message CryptoInfo. ...\n");
    sendmsg(sock_fd, &msg, 0);

    close(sock_fd);
}
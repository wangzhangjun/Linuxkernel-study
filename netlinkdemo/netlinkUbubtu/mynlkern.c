#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <net/netlink.h>

#define NETLINK_TEST 25 

typedef struct Security_module_options {
    int fs_options;
    int net_options;
    int usb_options;
    int port_options;
}SecurityModuleOptions;

struct sock *nl_sk = NULL;
SecurityModuleOptions recv_security_module_options;

void nl_data_ready( struct sk_buff *__skb ){
    struct sk_buff *skb;
	struct nlmsghdr *nlh; 

    printk("net_link: data is ready to read.\n");
    skb = skb_get(__skb);

    if (skb->len >= NLMSG_SPACE(0)) {
        nlh = nlmsg_hdr(skb);
        //接受用户态数据
        if(nlh && (nlh->nlmsg_len == NLMSG_SPACE(sizeof(SecurityModuleOptions)))){
            memcpy(&recv_security_module_options, NLMSG_DATA(nlh), sizeof(SecurityModuleOptions));
            printk("net_link: recv fs_options %d.\n", recv_security_module_options.fs_options);
            printk("net_link: recv net_options %d.\n", recv_security_module_options.net_options);
            printk("net_link: recv usb_options %d.\n", recv_security_module_options.usb_options);
            printk("net_link: recv port_options %d.\n", recv_security_module_options.port_options);
        }
        kfree_skb(skb);	
		return;
    }
}

struct netlink_kernel_cfg cfg = { 
  .input  = nl_data_ready, /* set recv callback */
};  

static int create_netlink(void){
    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if (!nl_sk) {
        printk(KERN_ERR "net_link: Cannot create netlink socket.\n");
        return -EIO;
    }
    printk("net_link: create socket ok.\n");
    return 0;
}

int init_module() {
    create_netlink();
    return 0;
}

void cleanup_module(){
  if (nl_sk != NULL){
    sock_release(nl_sk->sk_socket);
  }
  printk("net_link: remove ok.\n");
}

MODULE_LICENSE("GPL");
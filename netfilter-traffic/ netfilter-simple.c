#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>


unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    kuid_t	sock_uid;
    const struct file *filp;
    struct sock *sk;

    if (!in_interrupt()){
        printk(KERN_INFO "NO IN INTERRUPT\n");
    }

    // if(!skb) {
    //     printk(KERN_INFO "skb is null\n");
    //     return NF_ACCEPT;
    // }


    // if(skb->sk == NULL) {
    //     printk(KERN_INFO "skb->sk is null\n");
    //     return NF_ACCEPT;
    // }else{
    //     //printk(KERN_INFO "skb->sk is not null\n");
    //     sk = skb->sk;
    //     if(sk->sk_socket->file){
    //         sock_uid = filp->f_cred->fsuid;
    //         if(sock_uid.val != 0)
    //         {
    //             printk(KERN_INFO "sock_uid: %u\n",sock_uid.val);
    //         }
    //     }
    //     // if (!in_interrupt()){
    //     //     printk(KERN_INFO "no in interrupt\n");
    //     //     if(sk->sk_socket->file){
    //     //         printk(KERN_INFO "file is not null\n");
    //     //     }
    //     //         if((sk->sk_socket) && (sk->sk_socket->file))
    //     //         {
    //     //             filp = sk->sk_socket->file;
    //     //             sock_uid = filp->f_cred->fsuid;
    //     //             if(sock_uid.val != 0)
    //     //             {
    //     //                 printk(KERN_INFO "sock_uid: %u\n",sock_uid.val);
    //     //             }
    //     //         }
    //     // }
    // }
    return NF_ACCEPT;
}

unsigned int hook_func_pre(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    const struct net_device *el_dev;
    if(skb->dev) {
        el_dev = skb->dev;
    }

    if(el_dev->ieee80211_ptr == NULL){  //wifi
        printk(KERN_INFO "pre infname : %s\n",el_dev->name);
    }

    // printk(KERN_INFO "pre infname : %s\n",el_dev->name);  //pre-routing can get interface name

    return NF_ACCEPT;
}

unsigned int hook_func_in(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    const struct net_device *el_dev;
    if(skb->dev) {
        el_dev = skb->dev;
    }

    //printk(KERN_INFO "in infname : %s\n",el_dev->name);  //in-routing can get interface name

    return NF_ACCEPT;
}

unsigned int hook_func_out(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    const struct net_device *el_dev;
    if(skb->dev) {
        el_dev = skb->dev;
    }

    //printk(KERN_INFO "out infname : %s\n",el_dev->name);  //out-routing can not get interface name

    return NF_ACCEPT;
}

unsigned int hook_func_post(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    const struct net_device *el_dev;
    if(skb->dev) {
        el_dev = skb->dev;
    }

    if(el_dev->ieee80211_ptr == NULL){  //wifi
        printk(KERN_INFO "post infname : %s\n",el_dev->name);
    }

    // printk(KERN_INFO " : %s\n",el_dev->name);  //post-routing can get interface name

    return NF_ACCEPT;
}

static struct nf_hook_ops nfho_pre = {
    .hook       = (nf_hookfn *)hook_func_pre,   //一直处于中断中，拿不到uid，以及进程上下文信息
    .hooknum    = NF_INET_PRE_ROUTING,
    .pf         = PF_INET,
    .priority   = NF_IP_PRI_FIRST
};

static struct nf_hook_ops nfho_in = {
    .hook       = (nf_hookfn *)hook_func_in,
    .hooknum    = NF_INET_LOCAL_IN,
    .pf         = PF_INET,
    .priority   = NF_IP_PRI_FIRST
};

static struct nf_hook_ops nfho_out = {
    .hook       = (nf_hookfn *)hook_func_out,
    .hooknum    = NF_INET_LOCAL_OUT,
    .pf         = PF_INET,
    .priority   = NF_IP_PRI_FIRST
};

static struct nf_hook_ops nfho_post = {
    .hook       = (nf_hookfn *)hook_func_post,
    .hooknum    = NF_INET_POST_ROUTING,
    .pf         = PF_INET,
    .priority   = NF_IP_PRI_FIRST
};

static int __init onload(void) {
    nf_register_net_hook(&init_net,&nfho_pre);
    nf_register_net_hook(&init_net,&nfho_in);
    nf_register_net_hook(&init_net,&nfho_out);
    nf_register_net_hook(&init_net,&nfho_post);
    printk(KERN_EMERG "Loadable module initialized\n");

    return 0;
}

static void __exit onunload(void) {
    nf_unregister_net_hook(&init_net,&nfho_pre);
    nf_unregister_net_hook(&init_net,&nfho_in);
    nf_unregister_net_hook(&init_net,&nfho_out);
    nf_unregister_net_hook(&init_net,&nfho_post);
    printk(KERN_EMERG "Loadable module removed\n");
}


module_init(onload);
module_exit(onunload);
MODULE_LICENSE("GPL");

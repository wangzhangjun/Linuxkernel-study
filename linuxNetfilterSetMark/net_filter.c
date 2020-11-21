#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ip.h>
#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/moduleparam.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/kmod.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_nat_core.h>
#include <net/netfilter/nf_conntrack_ecache.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhangjun!");

static struct nf_hook_ops nfho_send;

unsigned int send_hook_func(void *priv, struct sk_buff *skb,const struct nf_hook_state *state)
{
    struct iphdr *iph;
    struct tcphdr *tcph = NULL;
    unsigned int dport = 0;

    if (!skb) {
        return NF_ACCEPT;
    }
    iph = (struct iphdr *) skb_network_header(skb);
    if (!iph) {
        printk("no ip header\n");
        return NF_ACCEPT;
    }
    if(iph->protocol == IPPROTO_TCP)
    {
        tcph = (struct tcphdr *) ((__u32 *) iph + iph->ihl);
        dport = htons((unsigned short int)tcph->dest);
        if (tcph->syn && dport == 12345) 
        {
            printk("tcp dest port is:%d\n", dport);
            skb->mark = 0x12345;
            enum ip_conntrack_info ctinfo;
            struct nf_conn *ct;
            ct = nf_ct_get(skb, &ctinfo);
            if(ct != NULL)
            {
                u_int32_t newmark;
                newmark = 0x12345;
               if (ct->mark != newmark) {
                    ct->mark = newmark;
                    nf_conntrack_event_cache(IPCT_MARK, ct);
                } 
            }
        }
    }
    return NF_ACCEPT;
}

static int init_send_hook(void)
{
    nfho_send.hook = (nf_hookfn *) send_hook_func;
    nfho_send.hooknum = NF_INET_LOCAL_OUT;
    nfho_send.pf = PF_INET;
    nfho_send.priority = NF_IP_PRI_FIRST;
    return nf_register_hook(&nfho_send);
    return 0;
}

int filter_init(void)
{
    int err = 0;
    if ((err = init_send_hook()))
        return err;
    return err;
}

int __init network_filter_init(void)
{
    int ret = 0;
    printk("init network filter\n");
    if ((ret = filter_init()) != 0) 
    {
         printk("filter_init error\n");
        return ret;
    }
    return 0;
}

void __exit network_filter_exit(void)
{
    printk("unload network filter driver!\n");
    nf_unregister_hook(&nfho_send);
    return;
}

module_init(network_filter_init);
module_exit(network_filter_exit);
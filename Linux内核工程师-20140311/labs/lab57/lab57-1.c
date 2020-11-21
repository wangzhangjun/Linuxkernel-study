#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

#include <linux/socket.h>
#include <linux/netdevice.h>

#include <net/ip.h>
#include <linux/skbuff.h>

static int jdo__netif_receive_skb(struct sk_buff *skb)
{
	struct sock *sk;
        struct sockaddr_pkt *spkt;

        if (skb)
        {
//             if (skb->mark == 1007 && skb->dev){
			printk("== mac =%x-%x-%x-%x-%x-%x<=%x-%x-%x-%x-%x-%x\n", skb->mac_header[0], skb->mac_header[1],skb->mac_header[2], skb->mac_header[3], skb->mac_header[4],skb->mac_header[5],skb->mac_header[6], skb->mac_header[7], skb->mac_header[8], skb->mac_header[9], skb->mac_header[10], skb->mac_header[11]);

			printk ("__netif_receive at netdev %s  packet, mac head len =%d ,len = %d\n",skb->dev->name, skb->data - skb_mac_header(skb),skb->len);
//	      }
	}


out:

        /* Always end with a call to jprobe_return(). */
        jprobe_return();
        return 0;
}
//setjmp-longjmp
static struct jprobe my_jprobe = {
	.entry = jdo__netif_receive_skb,
        .kp = {
                .symbol_name = "__netif_receive_skb", //"packet_rcv_spkt",  location at  /proc/kallsysms
        },
};

static int __init jprobe_init(void)
{
        int ret;

        ret = register_jprobe(&my_jprobe);
        if (ret < 0) {
                printk(KERN_INFO "register_jprobe failed, returned %d\n", ret);
                return -1;
        }
        printk(KERN_INFO "Planted jprobe at %p, handler addr %p\n",
               my_jprobe.kp.addr, my_jprobe.entry);
        return 0;
}

static void __exit jprobe_exit(void)
{
        unregister_jprobe(&my_jprobe);
        printk(KERN_INFO "jprobe at %p unregistered\n", my_jprobe.kp.addr);
}

module_init(jprobe_init)
module_exit(jprobe_exit)

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("Jprobe test case");
MODULE_LICENSE ("GPL v2");


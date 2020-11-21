/* **************** RHD236:VEL50_07_BETA s_30/lab2_nl_sender.c **************** */
/*
 * The code in the Solutions sets for the Axian Linux Developers'
 * Series is:

 * * * * * Copyright 1998-2009 by Axian Inc.  * * * * *

 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.

 * URL:    http://www.axian.com
 * email:  coop@axian.com 

 * The primary maintainer for this code is Jerry Cooperstein
 * (coop@axian.com).  The CONTRIBUTORS file (distributed with this
 * file) lists those known to have contributed to the source.

 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.

 */

/*
 *  Using Netlink to send kernel messages to an application.
 *  (SENDER MODULE)
 *
 * We give you a kernel module ( lab2_nl_sender.c) that sends messages
 * to a netlink socket to be monitored by a user application (
 * lab2_nl_receive_test.c).

 * Compile and test the two programs. Try sending multiple messages
 * through straightforward modifications.
 *
*/
/*
  Netlink demo that sends a message from kernel-space to user-space.

  Copyright Dominic Duval <dduval@redhat.com> according to the terms
  of the GNU Public License.

 (minor changes introduced by J. Cooperstein, coop@axian.com
  to work with newer kernels.)

 (major changes introduced by Paul Drews, paul.drews@intel.com)

*/

#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>

#define MAX_PAYLOAD 1024
#define NETLINK_MESSAGE "This message originated from the kernel!"
#define NL_EXAMPLE 19           /* was 19 */
#define NL_GROUP 1              /* was 1 */

#define MSG_INTERVAL_SEC 5

#define seconds_to_jiffies(sec) (cputime_to_jiffies(secs_to_cputime(sec)))

static struct sock *nl_sk = NULL;
static struct task_struct *sender_thread = NULL;

static int my_sender_thread (void *data)
{

    do {
        set_current_state (TASK_INTERRUPTIBLE);
	printk("helllo\n");
    } while (!kthread_should_stop ());

    return 0;

}                               /* my_sender_thread */

static int __init my_init (void)
{

    sender_thread = kthread_run (my_sender_thread,
                                 NULL, "lab2_nl_sender_thread");
    if (IS_ERR (sender_thread)) {
        printk ("Error %ld createing thread\n", PTR_ERR (sender_thread));
        return -1;
    }

    printk ("Adding netlink testing module\n");

    return 0;                   /* Always remove module immediately after loading */
}

static void __exit my_exit (void)
{
    printk ("Removing netlink testing module\n");
    kthread_stop (sender_thread);
}

module_exit (my_exit);
module_init (my_init);

MODULE_AUTHOR ("Dominic Duval");
MODULE_AUTHOR ("Paul Drews");
MODULE_DESCRIPTION ("RHD236:VEL50_07_BETA s_30/lab2_nl_sender.c");
MODULE_LICENSE ("GPL v2");

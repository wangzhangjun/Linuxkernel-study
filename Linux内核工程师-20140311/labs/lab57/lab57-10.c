/*
 * Extending SysRq keys.
 *
 * It is possible to extend they SysRq key mechanism to enable custom
 * defined keys and actions.  All the information needed can be found
 * in linux/sysrq.h.
 *
 * The relevant structures and functions are:
 *
 *  #include <linux/sysrq.h>
 *
 *  struct sysrq_key_op {
 *      void (*handler)(int, struct pt_regs *, struct tty_struct *);
 *      char *help_msg;
 *      char *action_msg;
 *  };
 *  int register_sysrq_key(int, struct sysrq_key_op *);
 *  int unregister_sysrq_key(int, struct sysrq_key_op *);
 *
 * where the key value (say 'x') is supplied as the first argument to
 * the handler function.
 *
 * Note that in the 2.6.19 kernel, the struct pt_regs argument is
 * dropped from the handler function.
 *
 * The purpose and usage of the other functions and variables should
 * be clear from the definition.
 *
 * Write a simple module that registers a new key, that can do as
 * little as print a message.  See what occurs when you get the help
 * message.
 *
 * Make sure you that you have turned this option on in your kernel at
 * compilation and in /proc/sys/kernel/sysrq.  If you have keyboard
 * problems you may need to do something like
 *
 *     echo x > /proc/sysrq-trigger
 *
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/sysrq.h>

#define MY_SYSRQ_KEY 'x'

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
static void my_sysrq_handler (int key, struct pt_regs *ptregs,
                              struct tty_struct *ttystruct)
#else
static void my_sysrq_handler (int key, struct tty_struct *ttystruct)
#endif
{
    printk ("Hello from function %s\n", __FUNCTION__);
    printk ("Key pressed was '%c'\n", key);
}

static struct sysrq_key_op my_sysrq_key_op = {
    .handler = my_sysrq_handler,
    .help_msg = "eXample",
    .action_msg = "eXample action message",
};

static int __init my_init (void)
{
    int rc = register_sysrq_key (MY_SYSRQ_KEY, &my_sysrq_key_op);

    if (rc == 0) {
        printk ("sysrq key registered\n");
    } else {
        printk ("sysrq key failed to register\n");
    }

    return rc;
}

static void __exit my_exit (void)
{
    int rc = unregister_sysrq_key (MY_SYSRQ_KEY, &my_sysrq_key_op);

    if (rc == 0) {
        printk ("sysrq key unregistered\n");
    } else {
        printk ("sysrq key failed to unregister\n");
    }
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("SysRq");
MODULE_LICENSE ("GPL v2");

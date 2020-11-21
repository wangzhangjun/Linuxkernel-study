#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/spinlock_types.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,26)
#define find_task_by_pid find_task_by_vpid
#endif

static int pid = -1;
static int signo = SIGABRT;
module_param (pid, int, 0);
module_param (signo, int, 0);
static __sighandler_t old_handler;
spinlock_t lock = SPIN_LOCK_UNLOCKED;
static int __init my_init (void)
{
    unsigned long flags;
    //struct task_struct *p = find_task_by_pid (pid);
    struct task_struct *p = pid_task(find_vpid(pid),PIDTYPE_PID);
    if (!p) {
        printk ("Invalid PID %d specified\n", pid);
        return -EINVAL;
    }
    //spin_lock_irqsave (&p->sighand->siglock, flags);
    spin_lock(&lock);
    old_handler = p->sighand->action[signo - 1].sa.sa_handler;
//    p->sighand->action[signo - 1].sa.sa_handler = SIG_DFL;
    p->sighand->action[signo - 1].sa.sa_handler = SIG_IGN;
    spin_unlock(&lock);
   // spin_unlock_irqrestore (&p->sighand->siglock, flags);

    printk ("Resetting process %s[%d] signal handler for %d to SIG_DFL\n",
            p->comm, p->pid, signo);
    return 0;
}

static void __exit my_exit (void)
{
    //struct task_struct *p = find_task_by_pid (pid);
    struct task_struct *p = pid_task(find_vpid(pid),PIDTYPE_PID);
    if (!p) {
        printk ("PID %d already gone!\n", pid);
        return;
    }
    spin_lock(&lock);
    p->sighand->action[signo - 1].sa.sa_handler = old_handler;
    spin_unlock(&lock);
    printk ("Resetting process %s[%d] signal handler for %d to SIG_DFL\n",
            p->comm, p->pid, signo);
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("spin lock test");
MODULE_LICENSE ("GPL v2");

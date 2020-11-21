#include <linux/module.h>
#include <linux/init.h>
#include <linux/unistd.h>

void *save_stub;

struct my_sc_struct
{
    long (*fun) (unsigned long p1);
};

extern struct my_sc_struct my_sc;

static long test_syscall (unsigned long p1)
{
    long rc;
    rc = 100 * p1;
    printk ("in test_syscall, p1 = %ld,rc = %ld\n", p1, rc);
    return rc;
}

static int __init my_init (void)
{
    struct my_sc_struct *s = &my_sc;
    printk ("Hit init_module\n");
    printk ("s->fun = %p\n", s->fun);
    save_stub = s->fun;
    s->fun = test_syscall;
    printk ("s->fun = %p\n", s->fun);
    return 0;
}

static void __exit my_exit (void)
{
    struct my_sc_struct *s = &my_sc;
    printk ("Hit cleanup_module\n");
    printk ("s->fun = %p\n", s->fun);
    s->fun = save_stub;
    printk ("Restored the original system call");
    printk ("s->fun = %p\n", s->fun);
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("Add a system call by kernel module");
MODULE_LICENSE ("GPL v2");

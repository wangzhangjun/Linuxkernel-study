#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/namei.h>
#include <linux/version.h>
#include <linux/pid.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhjwang");
MODULE_DESCRIPTION("test");
MODULE_VERSION("0.01");

static int __init init_oopsdemo(void)
{
    *((int *)0x00) = 0x12345678;
    return 0;
}

static void __exit exit_oopsdemo(void)
{
    return;
}
module_init(init_oopsdemo);
module_exit(exit_oopsdemo);
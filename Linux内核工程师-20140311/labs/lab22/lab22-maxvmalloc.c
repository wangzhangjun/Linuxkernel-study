/*
 * Testing Maximum Memory Allocation (vmalloc)
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/vmalloc.h>

static int mem = 128;
module_param (mem, int, S_IRUGO);

#define MB (1024*1024)

static int __init my_init (void)
{
    static char *vm_buff;
    int size;

    /* try vmalloc */

    for (size = 1 * MB; size <= mem * MB; size += 1 * MB) {
        printk (" pages=%6ld, size=%8d ", size / PAGE_SIZE, size / MB);
        if (!(vm_buff = (char *)vmalloc (size))) {
            printk ("... vmalloc failed\n");
            break;
        }
        printk ("... vmalloc OK\n");
        vfree (vm_buff);
    }

    return 0;
}
static void __exit my_exit (void)
{
    printk ("Module Unloading\n");
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("maxvmalloc test");
MODULE_LICENSE ("GPL v2");

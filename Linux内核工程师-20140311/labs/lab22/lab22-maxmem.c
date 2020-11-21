
/*
 * Testing Maximum Memory Allocation
 *
 * See how much memory you can obtain dynamically, using both
 * kmalloc() and __get_free_pages().
 *
 * Start with requesting 1 page of memory, and then keep doubling
 * until *your request fails for each type fails.
 *
 * Make sure you free any memory you receive.
 *
 * You'll probably want to use GFP_ATOMIC rather than
 * GFP_KERNEL. (Why?)

 * If you have trouble getting enough memory due to memory
 * fragmentation trying writing a poor-man's defragmenter, and then
 * running again.  The defragmenter can just be an application that
 * grabs all available memory, uses it, and then releases it when
 * done, thereby clearing the caches.  You can also try the command
 * sync; echo 3 > /proc/sys/vm/drop_caches .

 * Try the same thing with vmalloc().  Rather than
 * doubling allocations, start at 4 MB and increase in 4 MB incremements
 * until failure results.  Note this may hang while loading. (Why?)
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

static int __init my_init (void)
{
    static char *kbuff;
    static unsigned long order;
    int size;

    /* try __get_free_pages__ */

    for (size = PAGE_SIZE, order = 0; order < 20; order++, size *= 2) {
    //for (size = PAGE_SIZE, order = 0; order < MAX_ORDER; order++, size *= 2) {
        printk (" order=%2ld, pages=%5ld, size=%8d ", order,
                size / PAGE_SIZE, size);
        if (!(kbuff = (char *)__get_free_pages (GFP_ATOMIC, order))) {
            printk ("... __get_free_pages failed\n");
            break;
        }
        printk ("... __get_free_pages OK\n");
       // free_pages ((unsigned long)kbuff, order);
        free_pages ((unsigned long)kbuff, order);
    }

    /* try kmalloc */

    //for (size = PAGE_SIZE, order = 0; order < MAX_ORDER; order++, size *= 2) {
    for (size = PAGE_SIZE, order = 0; order < 20 ; order++, size *= 2) {
        printk (" order=%2ld, pages=%5ld, size=%8d ", order,
                size / PAGE_SIZE, size);
        if (!(kbuff = (char *)kmalloc ((size_t) size, GFP_ATOMIC))) {
            printk ("... kmalloc failed\n");
            break;
        }
        printk ("... kmalloc OK\n");
        kfree (kbuff);
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
MODULE_DESCRIPTION ("maxmem test");
MODULE_LICENSE ("GPL v2");


/*
 * Memory Pools
 *
 * Write a simple module that sets up a memory pool in the
 * initialization function.
 *
 * Make sure to request enough objects and memory per object to make
 * the memory usage obvious.
 *
 * Introduce a delay between requesting setting up the memory pool and
 * requesting the objects, so you can verify that the memory is
 * granted all at once initially. You can do this by not requesting
 * the memory until the cleanup function, or by introducing a delay.
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/mempool.h>
#include <linux/slab.h>

#define SIZE 16384
#define MIN_NR 10240
static int size = SIZE, min_nr = MIN_NR;

module_param (size, int, S_IRUGO);
module_param (min_nr, int, S_IRUGO);

static char **bufs;

static mempool_t *my_mempool;

static int __init my_init (void)
{
    my_mempool = mempool_create_kmalloc_pool (min_nr, size);
    printk ("my_mempool = %p\n", my_mempool);
    printk ("Hello: module loaded at 0x%p\n", my_init);
    return 0;
}
static void __exit my_exit (void)
{
    int j;

    /* should be something bigger ! */
    bufs = kmalloc (min_nr * sizeof (ulong), GFP_KERNEL);

    for (j = 0; j < min_nr; j++) {
        bufs[j] = mempool_alloc (my_mempool, GFP_ATOMIC);
        if (bufs[j]) {
               printk("bufs[ %d] = %p\n", j, bufs[j]);
        } else
            printk ("could not alloc bufs [%d]\n", j);
    }

    for (j = 0; j < min_nr; j++) {
        if (bufs[j]) {
            mempool_free (bufs[j], my_mempool);
            //            printk("Freeded bufs[%d]\n", j);
        } else {
            printk ("Problem freeeing: %d\n", j);
        }
    }

    mempool_destroy (my_mempool);
    /*    kfree (bufs); */
    printk ("Bye: module unloaded from 0x%p\n", my_exit);
}

module_init (my_init);
module_exit (my_exit);

MODULE_LICENSE ("GPL v2");
MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("mempool test");

/*
 * Making a Private Memory Cache.
 *
 * Write a module that allocates a buffer by using your own memory
 * cache.
 *
 * The size of the objects can be an optional parameter supplied when
 * loading the module.
 *
 * Set up the cache and allocate at least one object when loading the
 * module.
 *
 * Make sure you free up the object(s) and then the cache when
 * unloading the module.
 *
 * Make sure you free any objects you create, as well as the
 * memory cache itself. If you don't, the driver won't unload.
 */

#include <linux/module.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/version.h>

static char *kbuff;
static int size = PAGE_SIZE;
static struct kmem_cache *my_cache;
module_param (size, int, S_IRUGO);
//#define N 	1025 // failed
#define N       5 	//max 1024

static int __init my_init (void)
{
    /* create a memory cache */

    if (size > (32 * PAGE_SIZE)) {
        printk
            (" size=%d is too large; you can't have more than 32 pages!\n",
             size);
        return -1;
    }
// 4Byte <= object size <= 1024Pages (4M)
    if (!(my_cache = kmem_cache_create ("mycache", size * N, 0,
//    if (!(my_cache = kmem_cache_create ("mycache", 4, 0,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
                                        SLAB_HWCACHE_ALIGN, NULL, NULL))) {
#else
                                        SLAB_HWCACHE_ALIGN, NULL))) {
#endif
        printk ("kmem_cache_create failed\n");
        return -ENOMEM;
    }
    printk ("allocated memory cache correctly\n");

    /* allocate a memory cache object */

    if (!(kbuff = kmem_cache_alloc (my_cache, GFP_ATOMIC))) {
        printk (" failed to create a cache object\n");
        (void)kmem_cache_destroy (my_cache);
        return -1;
    }
    printk (" successfully created a cache object\n");
    return 0;
}

static void __exit my_exit (void)
{
    /* destroy a memory cache object */
    kmem_cache_free (my_cache, kbuff);
    printk ("destroyed a memory cache object\n");

    /* destroy the memory cache */
    (void)kmem_cache_destroy (my_cache);
}

module_init (my_init);
module_exit (my_exit);

MODULE_LICENSE ("GPL v2");
MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("cache test");

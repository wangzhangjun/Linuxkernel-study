
#include <linux/module.h>
#include <asm/atomic.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/rculist.h>

/* we're going to do a camel race! */
static LIST_HEAD (camel_list);

struct camel_entry
{
    struct list_head clist;     /* link in camel list */
    int gate;                   /* assigned gate at Camel Derby */
    char name[20];              /* camel's name */
};

static int __init my_init (void)
{
    struct camel_entry *ce;
    int k;

    for (k = 0; k < 5; k++) {

        if (!(ce = kmalloc (sizeof (struct camel_entry), GFP_KERNEL))) {
            printk (" Camels: failed to allocate memory for camel %d \n", k);
            return -ENOMEM;
        }

        ce->gate = 11 + k;      /* gate number */
        sprintf (ce->name, "Camel_%d", k + 1);
        printk (" Camels: adding %s at gate %d to camel_list \n",
                ce->name, ce->gate);
        list_add_rcu (&ce->clist, &camel_list);
    }
    return 0;
}

static void __exit my_exit (void)
{
    struct list_head *list;     /* pointer to list head object */
    struct list_head *tmp;      /* temporary list head for safe deletion */
    int i=0;

    if (list_empty(&camel_list)) {
        printk ("Camels (exit): camel list is empty! \n");
        return;
    }
    printk ("Camels: (exit): camel list is NOT empty! \n");

/*    list_for_each_entry_rcu(list,&camel_list,list_head) {
	for(i = 0;i < 5;i++)
	struct camel_entry *ce = list_entry_rcu (camel_list, struct camel_entry, clist);
        list_del (&ce->clist);
        printk ("Camels (exit): %s at gate %d removed from list \n",ce->name, ce->gate);
        kfree (ce);
 }*/

    /* Now, did we remove the camel manure? */

    if (list_empty(&camel_list))
       printk ("Camels (done): camel list is empty! \n");
    else
        printk ("Camels: (done): camel list is NOT empty! \n");
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("rcu test");
MODULE_LICENSE ("GPL v2");

/*
 * Modules and Exporting Symbols: Module 1
 *
 * Write a pair of modules where the second one calls a
 * function in the first one.
 *
 * Load them in the correct order and make sure all symbols are
 * resolved.
 *
*/

#include <linux/module.h>
#include <linux/init.h>
static unsigned long exported_symbol = 5;
static int __init my_init (void)
{
    printk ("Hello world from mod1 \n");
    return 0;
}

static void __exit my_exit (void)
{
    printk ("Goodbye world from mod1 \n");
}

static void mod1fun (void)
{
    printk (" Wa! I got into mod1fun!\n");
}

EXPORT_SYMBOL (mod1fun);
EXPORT_SYMBOL (exported_symbol);
module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION("This is mod1");
MODULE_LICENSE ("GPL v2");

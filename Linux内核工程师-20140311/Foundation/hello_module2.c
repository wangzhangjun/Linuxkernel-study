/*
 * Modules and Exporting Symbols: Module 2
 *
 * Write a pair of modules where the second one calls a
 * function in the first one.

 * Load them in the correct order and make sure all symbols are
 * resolved.
*/

#include <linux/module.h>
#include <linux/init.h>

extern void mod1fun (void);
extern unsigned long exported_symbol;
static int __init my_init (void)
{
    	printk("Hello world from mod2\n");
	printk("exported_symbol is %d\n",exported_symbol);	
    	mod1fun();
    	return 0;
}

static void __exit my_exit (void)
{
    	printk ("Goodbye world from mod2\n");
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("This is mod2");
MODULE_LICENSE ("GPL v2");

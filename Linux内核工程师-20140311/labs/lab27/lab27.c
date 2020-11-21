#include <linux/module.h>
#include <linux/init.h>


static int __init example_init(void)          
{
            int val = 0;

            atomic_t my_counter=ATOMIC_INIT(0);
    
            atomic_set( &my_counter, 3 );
            atomic_add( 2, &my_counter );

            val = atomic_read( &my_counter );

            printk("get atomic_value :%d\n",val);
            return 0;

 }
static void __exit example_exit(void)
{

	printk("end\n");
}
module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION (" atomic operation test");
MODULE_LICENSE ("GPL v2");

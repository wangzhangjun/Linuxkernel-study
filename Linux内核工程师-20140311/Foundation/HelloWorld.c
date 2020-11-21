#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
static int hello_init(void)
{
	printk(KERN_INFO "Hello, world!\n");
	printk ("Hello: module loaded at 0x%p\n",hello_init);
	printk("HZ = %d\n",HZ);
	return 0;
}
static void  hello_exit(void)
{
	printk(KERN_INFO "Goodbye, cruel world!\n");
	printk ("Bye: module loaded at 0x%p\n",hello_exit);
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION("Hello world,first kernel module");

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/delay.h>

#define RD_ADDR	  0x02f8
static int write_init(void)
{
	unsigned long count=0;
	unsigned  char status; 
	printk("This is a write module \n");
//	while(1)
//	{
		outw(0x34,RD_ADDR);
//		msleep(10);
		//msleep_interruptible(10);
//		count++;
//	}
	return 0;

}


static void write_exit(void)
{

	printk("Bye \n");

}
module_init(write_init);
module_exit(write_exit);
MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("I/O port test");
MODULE_LICENSE("GPL");

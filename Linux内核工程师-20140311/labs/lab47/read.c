#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/delay.h>

#define RD_ADDR	  0x02f8
static int read_init(void)
{
	unsigned long count=0;
	unsigned  char status; 
	printk("This is a read module \n");
//	while(1)
//	{
		status = inw(RD_ADDR);
		printk("status = %d,count = %d\n",status,count);
//		msleep(10);
		//msleep_interruptible(10);
//		count++;
//	}
	return 0;

}


static void read_exit(void)
{

	printk("Bye \n");

}
module_init(read_init);
module_exit(read_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("I/O port test");
MODULE_LICENSE("GPL");

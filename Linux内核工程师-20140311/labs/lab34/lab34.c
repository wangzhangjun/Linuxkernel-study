#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sem.h>
#include <linux/delay.h>

struct completion my_completion1;
struct completion my_completion2;
int thread_dirver(void *);
int thread_saleman(void *);

int thread_driver(void *p)
{
    printk(KERN_ALERT"DRIVER:I AM WAITING FOR SALEMAN CLOSED THE DOOR\n");
    wait_for_completion(&my_completion1);
    printk("my_completion1.done = %d\n",my_completion1.done);

    printk(KERN_ALERT"DRIVER:OK , LET'S GO!NOW~\n");
    printk(KERN_ALERT"DRIVER:ARRIVE THE STATION.STOPED CAR!\n");

    complete(&my_completion2);
    printk("my_completion2.done = %d\n",my_completion2.done);
     return 0;
}

int thread_saleman(void *p)//ÊÛÆ±Ô±Ïß³Ì
{
    printk(KERN_ALERT"SALEMAN:THE DOOR IS CLOSED!\n");
    complete(&my_completion1);
    printk("my_completion1.done = %d\n",my_completion1.done);

    printk(KERN_ALERT"SALEMAN:YOU CAN GO NOW\n");
    wait_for_completion(&my_completion2);
    printk("my_completion2.done = %d\n",my_completion2.done);
    printk(KERN_ALERT"SALEMAN:OK,THE DOOR BE OPENED!\n");
    return 0;
}

static int hello_init(void)
{
	int ret;
    	printk(KERN_ALERT"Hello everybody~\n");
    	init_completion(&my_completion1);
    	init_completion(&my_completion2);
	printk("my_completion1.done = %d\n",my_completion1.done);
	printk("my_completion2.done = %d\n",my_completion2.done);

	kernel_thread(thread_saleman,NULL,CLONE_KERNEL);
    	kernel_thread(thread_driver,NULL,CLONE_KERNEL);
   
        return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT"sunqidong Goodbye everybody~\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION("A simple completion Module");
MODULE_LICENSE("Dual BSD/GPL");

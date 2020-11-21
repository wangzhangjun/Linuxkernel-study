#include <linux/module.h>   
#include <linux/kernel.h>   
#include <linux/init.h>   
#include <linux/interrupt.h>   
#include <linux/timer.h>    
#include <linux/delay.h>    
#include <linux/hardirq.h>


#define DEBUG   
  
#ifdef DEBUG   
#define MSG(message, args...) printk(KERN_DEBUG "irq:" message, ##args)   
#else   
#define MSG(message, args...)   
#endif   
  
  
int irq;   
char *interface;
static int count = 1;   
module_param(irq, int, 0644);   
module_param(interface, charp, 0644);   
int irq_handle_function(int irq, void *device_id)   
{   
	if(count % 10 == 0)
		{
    			MSG("[%d]receive the irq at %ld...\n", count, jiffies); 
			printk("in_interupt(irq) = %d\n",in_interrupt());
		}
	//msleep(1); dead
	//ndelay(1); ok
	//mdelay(1);// ok
    	count++;   
    	return IRQ_NONE;   
}   
  
int init_module()   
{   
 	if(request_irq(irq, irq_handle_function, IRQF_SHARED, interface, (void *)&irq))   
    	{   
        	MSG("regist irq failure...\n");   
        	return -EIO;   
    	}   
	printk("in_interupt = %d\n",in_interrupt());
    	MSG("interface=%s and irq=%d...\n", interface, irq);   
    	MSG("register irq success...\n");   
    	return 0;   
}   
void cleanup_module()   
{   
    	free_irq(irq, &irq);   
    	MSG("unregister irq...\n");   
}  
MODULE_LICENSE("GPL");
MODULE_AUTHOR("support@ingben.com");   
MODULE_DESCRIPTION ("irq test");

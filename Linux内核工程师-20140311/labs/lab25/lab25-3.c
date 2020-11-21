#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/kthread.h>

struct work_struct *my_wq;
static struct task_struct *my_thread = NULL;
static void  my_do_work()
{
	printk("This a my_do_work\n");
}

static int my_do_thread (void )
{

    while(!kthread_should_stop()) {
      //  set_current_state (TASK_INTERRUPTIBLE);
        schedule_work(&my_wq);
    }

    return 0;

}   
int __init init_module(void)
{
	int result = 0;
	my_wq= ( struct work_struct *)kmalloc(sizeof(struct  work_struct), GFP_KERNEL);
    	INIT_WORK((struct work_struct *)my_wq, my_do_work);
	my_thread = kthread_run (my_do_thread,NULL, "my_do_thread_test");
    	if (IS_ERR (my_thread)) 
	{
        	printk ("Error %ld createing thread\n", PTR_ERR (my_thread));
        	return -1;
    	}
	printk("init ok\n");
}
 


void __exit clean_module(void)
{
	kthread_stop (my_thread);
	printk("exit ok");
}


MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION (" work queue of buttom half test");
MODULE_LICENSE ("GPL v2");


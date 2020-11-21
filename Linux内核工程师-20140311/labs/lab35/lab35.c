#include <linux/kthread.h>   
#include <linux/module.h>   
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/seqlock.h>

#ifndef SLEEP_MILLI_SEC   
#define SLEEP_MILLI_SEC(nMilliSec)\   
do { \  
	long timeout = (nMilliSec) * HZ / 1000; \  
	while(timeout > 0) \  
	{ \  
		timeout = schedule_timeout(timeout); \  
	} \  
}while(0);  
#endif   
static struct task_struct * MyThread_a = NULL;  
static struct task_struct * MyThread_b = NULL;  
static struct task_struct * MyThread_c = NULL;  
rwlock_t my_rw_spin_lock;
static int shared_res = 0;
seqlock_t my_seq_lock;



static int MyPrintk_c(void *data)  
{  
	char *mydata = kmalloc(strlen(data)+1,GFP_KERNEL);  
	memset(mydata,'\0',strlen(data)+1);  
	strncpy(mydata,data,strlen(data));  
	while(!kthread_should_stop())  
	{  
		SLEEP_MILLI_SEC(1000);  
		printk("%s\n",mydata);  
		
		write_seqlock(&my_seq_lock);	
	        shared_res++;
        	printk("%s: shared_res  = %d\n", __FUNCTION__, shared_res );
		printk("my_seq_lock.sequence = %d\n",my_seq_lock.sequence);
		write_sequnlock(&my_seq_lock);	


	}  
	kfree(mydata);  
	return 0;  
}  

static int MyPrintk_b(void *data)  
{  
	char *mydata = kmalloc(strlen(data)+1,GFP_KERNEL);  
	memset(mydata,'\0',strlen(data)+1);  
	strncpy(mydata,data,strlen(data));  
	while(!kthread_should_stop())  
	{  
		SLEEP_MILLI_SEC(1000);  
		printk("%s\n",mydata);  
		
		write_seqlock(&my_seq_lock);	
	        shared_res++;
        	printk("%s: shared_res  = %d\n", __FUNCTION__, shared_res );
		printk("my_seq_lock.sequence = %d\n",my_seq_lock.sequence);
		write_sequnlock(&my_seq_lock);	
	}  
	kfree(mydata);  
	return 0;  
}  

static int MyPrintk_a(void *data)  
{  
	int val = 0;
	unsigned int seq_num = 0;
	char *mydata = kmalloc(strlen(data)+1,GFP_KERNEL);  
	memset(mydata,'\0',strlen(data)+1);  
	strncpy(mydata,data,strlen(data));  
	while(!kthread_should_stop())  
	{  
		SLEEP_MILLI_SEC(1000);  
		printk("%s\n",mydata);  

		do
		{
			seq_num = read_seqbegin(&my_seq_lock);
	        	val = shared_res;
        		printk("%s: shared_res  = %d\n", __FUNCTION__, val);
			printk("my_seq_lock.sequence = %d,seq_num = %d\n",my_seq_lock.sequence,seq_num);
		}while(read_seqretry(&my_seq_lock, seq_num));
	}  
	kfree(mydata);  
	return 0;  
}  
static int __init init_kthread(void)  
{ 
//	my_rw_spin_lock = RW_LOCK_UNLOCKED; 
//	rwlock_init(&my_rw_spin_lock);
	seqlock_init(&my_seq_lock);
	printk("my_seq_lock.sequence = %d\n",my_seq_lock.sequence);
	printk("init seq lock ok\n");
	MyThread_a = kthread_run(MyPrintk_a,"hello world a","mythread_a");  
	MyThread_b = kthread_run(MyPrintk_b,"hello world b","mythread_b");  
	MyThread_c = kthread_run(MyPrintk_c,"hello world c","mythread_b");  
	return 0;  
}  
static void __exit exit_kthread(void)  
{  
	if(MyThread_a)  
	{  
		printk("stop MyThread_a\n");  
		kthread_stop(MyThread_a);  
	}  
	if(MyThread_b)  
	{  
		printk("stop MyThread_b\n");  
		kthread_stop(MyThread_b);  
	}  
	if(MyThread_c)  
	{  
		printk("stop MyThread_c\n");  
		kthread_stop(MyThread_c);  
	}  
}  
module_init(init_kthread);  
module_exit(exit_kthread);  
 
MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("Seq locks test");
MODULE_LICENSE ("GPL v2");


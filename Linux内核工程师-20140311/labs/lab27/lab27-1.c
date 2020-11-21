#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>

#include <asm/atomic.h>


static struct task_struct* my_thread1 = NULL;
static struct task_struct* my_thread2 = NULL;
static struct task_struct* my_thread3 = NULL;

static int sleep_time = (1*10*HZ);
static atomic_t shared_res;
static unsigned int bit_var = 31; //0x1f

static int thread_process1(void* param)
{
	int val = 0, ret = 0;
  	while(1)
  	{
    		set_current_state(TASK_UNINTERRUPTIBLE);
    		if(kthread_should_stop())
    		{
      			printk("kernel thread '%s' should stop;file:%s;line:%d\n", __FUNCTION__, __FILE__, __LINE__);
      			break;
    		}
    		set_bit(0, (volatile void*)&bit_var); // 00011111 -> 00011111
    		printk("%s: set bit_var = %u;\n", __FUNCTION__, bit_var);
    		clear_bit(0, (volatile void*)&bit_var); //00011111 -> 00011110
    		printk("%s: clear bit_var = %u;\n", __FUNCTION__, bit_var);
    		//atomic_add(1, &shared_res);
       		//val = atomic_read(&shared_res);
            	//val = atomic_add_return(1, &shared_res);
                //val = atomic_inc_return(&shared_res);
     		ret = atomic_inc_and_test(&shared_res);
     		val = atomic_read(&shared_res);
     		printk("%s: shared resource = %d, true=%d;\n%s", __FUNCTION__, val, ret, ((val % 3) ? "" : "\n"));
     		mdelay(sleep_time);
        }
    	return 123;
};
    

static int thread_process2(void* param)
{
  	int val = 0, ret = 0;
  	while(1)
  	{
    		set_current_state(TASK_UNINTERRUPTIBLE);
    		if(kthread_should_stop())
    		{
      			printk("kernel thread '%s' should stop;file:%s;line:%d\n", __FUNCTION__, __FILE__, __LINE__);
      			break;
    		}
    		set_bit(5, (volatile void*)&bit_var);//00011111 -> 00111111
    		printk("%s: set bit_var = %u;\n", __FUNCTION__, bit_var);
    		clear_bit(4, (volatile void*)&bit_var);//00011111->00001111
    		printk("%s: clear bit_var = %u;\n", __FUNCTION__, bit_var);
   		//atomic_add(1, &shared_res);
    		//val = atomic_read(&shared_res);
        	//val = atomic_add_return(1, &shared_res);
            	//val = atomic_inc_return(&shared_res);
		ret = atomic_inc_and_test(&shared_res);
    		val = atomic_read(&shared_res);
    		printk("%s: shared resource = %d, true=%d;\n%s", __FUNCTION__, val, ret, ((val % 3) ? "" : "\n"));
    		msleep(sleep_time);
  	}
  	return 456;
};

static int thread_process3(void* param)
{
  	int val = 0, ret = 0;
  	while(1)
  	{
    		set_current_state(TASK_UNINTERRUPTIBLE);

    		if(kthread_should_stop())
    		{
      			printk("kernel thread '%s' should stop;file:%s;line:%d\n", __FUNCTION__, __FILE__, __LINE__);
      			break;
    		}
    		set_bit(3, (volatile void*)&bit_var);
    		printk("%s: set bit_var = %u;\n", __FUNCTION__, bit_var);
    		clear_bit(2, (volatile void*)&bit_var);
    		printk("%s: clear bit_var = %u;\n", __FUNCTION__, bit_var);
 		//atomic_add(1, &shared_res);
     		//val = atomic_read(&shared_res);
         	//val = atomic_add_return(1, &shared_res);
             	//val = atomic_inc_return(&shared_res);
  		ret = atomic_inc_and_test(&shared_res);
    		val = atomic_read(&shared_res);
    		printk("%s: shared resource = %d, true=%d;\n%s", __FUNCTION__, val, ret, ((val % 3) ? "" : "\n"));
    		msleep(sleep_time);
  	}
  	return 789;
};

static int __init demo_init(void)
{
 	int err = 0;
 	printk("%s\n", __PRETTY_FUNCTION__);

	//shared_res = ATOMIC_INIT(0);
 	//printk("int-atomic ATOMIC_INIT=%d\n", atomic_read(&shared_res));
 	atomic_set(&shared_res, -100);
 	printk("int-atomic atomic_set=%d\n", atomic_read(&shared_res));

 	my_thread1 = kthread_create(thread_process1, NULL, "my_thread1");
 	if(IS_ERR(my_thread1))
 	{
   		err = PTR_ERR(my_thread1);
   		my_thread1 = NULL;
   		printk(KERN_ERR "unable to start kernel thread1:%d\n", err);
   		return err;
 	}

	my_thread2 = kthread_create(thread_process2, NULL, "my_thread2");
 	if(IS_ERR(my_thread2))
 	{
   		err = PTR_ERR(my_thread2);
   		my_thread2 = NULL;
   		printk(KERN_ERR "unable to start kernel thread2:%d\n", err);
   		return err;
 	}

	my_thread3 = kthread_create(thread_process3, NULL, "my_thread3");
 	if(IS_ERR(my_thread3))
 	{
   		err = PTR_ERR(my_thread3);
   		my_thread3 = NULL;
   		printk(KERN_ERR "unable to start kernel thread3:%d\n", err);
   		return err;
 	}

 	wake_up_process(my_thread1);
	wake_up_process(my_thread2);
	wake_up_process(my_thread3);
 	printk("%s:all kernel thread start;\n", __FUNCTION__);
 	return 0;
}

static void __exit demo_exit(void)
{
 	int ret = -1;
 	printk("%s\n",__PRETTY_FUNCTION__);

 	if(my_thread1)
 	{
   		ret = kthread_stop(my_thread1);
   		my_thread1 = NULL;
   		printk("kernel thread1 stop,exit code is %d;\n",ret);
 	}

 	if(my_thread2)
 	{
   		ret = kthread_stop(my_thread2);
   		my_thread2 = NULL;
   		printk("kernel thread2 stop,exit code is %d;\n",ret);
 	}

 	if(my_thread3)
 	{
   		ret = kthread_stop(my_thread3);
   		my_thread3 = NULL;
   		printk("kernel thread3 stop,exit code is %d;\n",ret);
 	}

 	printk("%s:all kernel thread stop;\n", __FUNCTION__);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION (" simulate conflux atomic operation test");
MODULE_LICENSE ("GPL v2");   

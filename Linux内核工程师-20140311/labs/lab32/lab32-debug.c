#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>

//#include <linux/rwlock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("*************");
MODULE_VERSION("2.6.35.000");

static int sleep_time = (1*10*HZ);
static int shared_res = 0;

rwlock_t my_rw_spin_lock;

static void thread_process1(void)
{
while(1)
  {
//    set_current_state(TASK_UNINTERRUPTIBLE);
/*
    if(kthread_should_stop())
    {
      printk("kernel thread '%s' should stop;file:%s;line:%d\n", __FUNCTION__, __FILE__, __LINE__);
      break;
    }
*/
	write_lock(&my_rw_spin_lock);
    	shared_res++;
    	printk("%s: shared resource = %d\n", __FUNCTION__, shared_res );
 	write_unlock(&my_rw_spin_lock);

    	mdelay(sleep_time);
  }
  //return 12;
};

static void  thread_process2(void)
{
while(1)
  {
  //  set_current_state(TASK_UNINTERRUPTIBLE);
/*
    if(kthread_should_stop())
    {
      printk("kernel thread '%s' should stop;file:%s;line:%d\n", __FUNCTION__, __FILE__, __LINE__);
      break;
    }
*/
 	write_lock(&my_rw_spin_lock);
    	shared_res++;
    	printk("%s: shared resource = %d\n", __FUNCTION__, shared_res );
	write_unlock(&my_rw_spin_lock);

    	msleep(sleep_time);
  }
 // return 34;
};

static void  thread_process3(void)
{
int val = 0;//, ret = 0;

while(1)
  {
   // set_current_state(TASK_UNINTERRUPTIBLE);
/*
    if(kthread_should_stop())
    {
      printk("kernel thread '%s' should stop;file:%s;line:%d\n", __FUNCTION__, __FILE__, __LINE__);
      break;
    }
*/
	read_lock(&my_rw_spin_lock);
    	val = shared_res;
    	printk("%s: shared resource = %d\n", __FUNCTION__, val);
	read_unlock(&my_rw_spin_lock);

    	msleep(sleep_time);
  }
 // return 56;
};

static void  thread_process4(void)
{
int val = 0;//, ret = 0;
while(1)
  {
 //   set_current_state(TASK_UNINTERRUPTIBLE);
/*
    if(kthread_should_stop())
    {
      printk("kernel thread '%s' should stop;file:%s;line:%d\n", __FUNCTION__, __FILE__, __LINE__);
      break;
    }
*/
	read_lock(&my_rw_spin_lock);
    	val = shared_res;
    	printk("%s: shared resource = %d\n", __FUNCTION__, val);
 	read_unlock(&my_rw_spin_lock);

    	msleep(sleep_time);
  }
 // return 78;
};

static struct task_struct* my_thread1 = NULL;
static struct task_struct* my_thread2 = NULL;
static struct task_struct* my_thread3 = NULL;
static struct task_struct* my_thread4 = NULL;

static int __init study_init(void)
{
 int err = 0;
 printk("%s\n", __PRETTY_FUNCTION__);

rwlock_init(&my_rw_spin_lock);
 printk("init spin lock ok\n");

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

 my_thread4 = kthread_create(thread_process4, NULL, "my_thread4");
 if(IS_ERR(my_thread4))
 {
   err = PTR_ERR(my_thread4);
   my_thread4 = NULL;
   printk(KERN_ERR "unable to start kernel thread4:%d\n", err);
   return err;
 }

 wake_up_process(my_thread1);
 wake_up_process(my_thread2);
 wake_up_process(my_thread3);
 wake_up_process(my_thread4);
 printk("%s:all kernel thread start;\n", __FUNCTION__);
 return 0;
}

static void __exit study_exit(void)
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

 if(my_thread4)
 {
   ret = kthread_stop(my_thread4);
   my_thread4 = NULL;
   printk("kernel thread4 stop,exit code is %d;\n",ret);
 }

 printk("%s:all kernel thread stop;\n", __FUNCTION__);
}

module_init(study_init);
module_exit(study_exit);


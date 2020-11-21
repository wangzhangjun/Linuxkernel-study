#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>

#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("*************");
MODULE_VERSION("2.6.35.000");

static int flag = 0;

void my_do_work(struct work_struct* work)
{
  flag = 1;
  printk("%s:hello, my work queue function\n", __FUNCTION__);
};

static struct work_struct my_work;

static int thread_process(void* param)
{
  while(1)
  {
    set_current_state(TASK_UNINTERRUPTIBLE);

    if(kthread_should_stop())
    {
      printk("kernel thread should stop;file:%s;line:%d\n", __FILE__, __LINE__);
      break;
    }

    mdelay(10*HZ);

 if(0 == flag)
    {
      schedule_work(&my_work);
    }
    else
    {
      flag = 0;
    }
  }
  return 123;
};

static struct task_struct* my_thread = NULL;

static int __init study_init(void)
{
 int err = 0;
 printk("%s\n", __PRETTY_FUNCTION__);

INIT_WORK(&my_work, my_do_work);

 //my_thread = kthread_create(thread_process, NULL, "my_thread");
 my_thread = kthread_run(thread_process, NULL, "my_thread");
 if(IS_ERR(my_thread))
 {
   err = PTR_ERR(my_thread);
   my_thread = NULL;
   printk(KERN_ERR "unable to start kernel thread:%d\n", err);
   return err;
 }

// wake_up_process(my_thread);
 printk("kernel thread start;file:%s;line:%d\n", __FILE__, __LINE__);
 return 0;
}

static void __exit study_exit(void)
{
 int ret = -1;
 printk("%s\n",__PRETTY_FUNCTION__);

 if(my_thread)
 {
   ret = kthread_stop(my_thread);
   my_thread = NULL;
 }
 printk("kernel thread stop,exit code is %d;file:%s;line:%d\n",ret, __FILE__, __LINE__);
}

module_init(study_init);
module_exit(study_exit);


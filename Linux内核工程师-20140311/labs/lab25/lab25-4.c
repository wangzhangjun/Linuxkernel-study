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

static int flag = 0;

void my_udf_do_work(struct work_struct* work)
{
  flag = 1;
  printk("%s:hello, my udf work queue function\n", __FUNCTION__);
};

static struct work_struct my_udf_work;

static struct workqueue_struct* lp_wq = NULL;

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

    //STEP6:µ÷¶È¹¤×÷½á¹¹Ìå¶ÔÏóÖ´ÐÐ:°Ñ¹¤×÷½á¹¹Ìå¶ÔÏóÌí¼Óµ½×Ô¼º´´½¨µÄ¹¤×÷¶ÓÁÐÖÐµÈ´ýÖ´ÐÐ;
        if(0 == flag)
            {
                 queue_work(lp_wq, &my_udf_work);
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

INIT_WORK(&my_udf_work, my_udf_do_work);

lp_wq = NULL;
 lp_wq = create_workqueue("my_work_queue");
 if(!lp_wq)
 {
   printk(KERN_ERR "create my work queueu failed\n");
   return -1;
 }

 my_thread = kthread_create(thread_process, NULL, "my_thread");
 if(IS_ERR(my_thread))
 {
   err = PTR_ERR(my_thread);
   my_thread = NULL;
   printk(KERN_ERR "unable to start kernel thread:%d\n", err);
   return err;
 }

 wake_up_process(my_thread);
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

destroy_workqueue(lp_wq);
 lp_wq = NULL;

 printk("kernel thread stop,exit code is %d;file:%s;line:%d\n",ret, __FILE__, __LINE__);
}

module_init(study_init);
module_exit(study_exit);

   
MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION (" work queue of buttom half test");
MODULE_LICENSE ("GPL v2");

   

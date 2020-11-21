#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/semaphore.h>

struct semaphore sem1;
struct semaphore sem2;

int num[2][5] = {
       {0,2,4,6,8},
       {1,3,5,7,9}
};
int thread_one(void *p);
int thread_two(void *p);

int thread_one(void *p)
{
	printk("This is thread one\n");	
       int *num = (int *)p;
       int i;
       for(i = 0; i < 5; i++){
              down(&sem1);      //»ñÈ¡ÐÅºÅÁ¿1
              printk(" %d ", num[i]);
              up(&sem2);    //ÊÍ·ÅÐÅºÅÁ¿2
	printk("thread_one:\n");
       }
       return 0;
}

int thread_two(void *p)
{

	printk("This is thread two\n");	
       int *num = (int *)p;
       int i;
       for(i = 0; i < 5; i++){
              down(&sem2);             //»ñÈ¡ÐÅºÅÁ¿2
              printk(" %d ", num[i]);
              up(&sem1);           //ÊÍ·ÅÐÅºÅÁ¿1
	printk("thread_two:\n");
       }
       return 0;
}

static int demo_init(void)
{
       printk("demo_init \n");
       init_MUTEX(&sem1);  //³õÊ¼»¯ÐÅºÅÁ¿1£¬ Ê¹ÐÅºÅÁ¿1×î³õ¿É±»»ñÈ¡
       init_MUTEX_LOCKED(&sem2);  //³õÊ¼»¯ÐÅºÅÁ¿2£¬Ê¹ÐÅºÅÁ¿2Ö»ÓÐ±»ÊÍ·Åºó²Å¿É±»»ñÈ¡
       kernel_thread(thread_one, num[0], CLONE_KERNEL);
       kernel_thread(thread_two, num[1], CLONE_KERNEL);
       return 0;
}

static void demo_exit(void)
{
       printk("demo_exit\n");
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("semaphore test");
MODULE_LICENSE ("GPL v2");


